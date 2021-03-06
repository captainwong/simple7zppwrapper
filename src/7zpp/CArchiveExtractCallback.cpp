#include "stdafx.h"
#include "CArchiveExtractCallback.h"

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64 size)
{
	dprintf("CArchiveExtractCallback::SetTotal %llu\n", size);
	total_ = size;
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64* completeValue)
{
	dprintf("CArchiveExtractCallback::SetCompleted %llu\n", *completeValue);
	if (callback_) {
		callback_(*completeValue, total_);
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode)
{
	*outStream = 0;
	_outFileStream.Release();

	{
		// Get Name
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop));

		UString fullPath;
		if (prop.vt == VT_EMPTY)
			fullPath = kEmptyFileAlias;
		else {
			if (prop.vt != VT_BSTR)
				return E_FAIL;
			fullPath = prop.bstrVal;
		}
		_filePath = fullPath;
	}

	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		return S_OK;

	{
		// Get Attrib
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop));
		if (prop.vt == VT_EMPTY) {
			_processedFileInfo.Attrib = 0;
			_processedFileInfo.AttribDefined = false;
		} else {
			if (prop.vt != VT_UI4)
				return E_FAIL;
			_processedFileInfo.Attrib = prop.ulVal;
			_processedFileInfo.AttribDefined = true;
		}
	}

	RINOK(IsArchiveItemFolder(_archiveHandler, index, _processedFileInfo.isDir));

	{
		// Get Modified Time
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop));
		_processedFileInfo.MTimeDefined = false;
		switch (prop.vt) {
		case VT_EMPTY:
			// _processedFileInfo.MTime = _utcMTimeDefault;
			break;
		case VT_FILETIME:
			_processedFileInfo.MTime = prop.filetime;
			_processedFileInfo.MTimeDefined = true;
			break;
		default:
			return E_FAIL;
		}

	}
	{
		// Get Size
		NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
		UInt64 newFileSize;
		/* bool newFileSizeDefined = */ ConvertPropVariantToUInt64(prop, newFileSize);
	}


	{
		// Create folders for file
		int slashPos = _filePath.ReverseFind_PathSepar();
		if (slashPos >= 0)
			CreateComplexDir(_directoryPath + us2fs(_filePath.Left(slashPos)));
	}

	FString fullProcessedPath = _directoryPath + us2fs(_filePath);
	_diskFilePath = fullProcessedPath;

	if (_processedFileInfo.isDir) {
		CreateComplexDir(fullProcessedPath);
	} else {
		NFind::CFileInfo fi;
		if (fi.Find(fullProcessedPath)) {
			if (!DeleteFileAlways(fullProcessedPath)) {
				PrintError("Can not delete output file", fullProcessedPath);
				return E_ABORT;
			}
		}

		_outFileStreamSpec = new COutFileStream;
		CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
		if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS)) {
			PrintError("Can not open output file", fullProcessedPath);
			return E_ABORT;
		}
		_outFileStream = outStreamLoc;
		*outStream = outStreamLoc.Detach();
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
	_extractMode = false;
	switch (askExtractMode) {
	case NArchive::NExtract::NAskMode::kExtract:  _extractMode = true; break;
	};
	switch (askExtractMode) {
	case NArchive::NExtract::NAskMode::kExtract:  Print(kExtractingString); break;
	case NArchive::NExtract::NAskMode::kTest:  Print(kTestingString); break;
	case NArchive::NExtract::NAskMode::kSkip:  Print(kSkippingString); break;
	};
	Print(_filePath);
	return S_OK;
}


STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
	switch (operationResult) {
	case NArchive::NExtract::NOperationResult::kOK:
		break;
	default:
		{
			NumErrors++;
			Print("  :  ");
			const char* s = NULL;
			switch (operationResult) {
			case NArchive::NExtract::NOperationResult::kUnsupportedMethod:
				s = kUnsupportedMethod;
				break;
			case NArchive::NExtract::NOperationResult::kCRCError:
				s = kCRCFailed;
				break;
			case NArchive::NExtract::NOperationResult::kDataError:
				s = kDataError;
				break;
			case NArchive::NExtract::NOperationResult::kUnavailable:
				s = kUnavailableData;
				break;
			case NArchive::NExtract::NOperationResult::kUnexpectedEnd:
				s = kUnexpectedEnd;
				break;
			case NArchive::NExtract::NOperationResult::kDataAfterEnd:
				s = kDataAfterEnd;
				break;
			case NArchive::NExtract::NOperationResult::kIsNotArc:
				s = kIsNotArc;
				break;
			case NArchive::NExtract::NOperationResult::kHeadersError:
				s = kHeadersError;
				break;
			}
			if (s) {
				Print("Error : ");
				Print(s);
			} else {
				char temp[16];
				ConvertUInt32ToString(operationResult, temp);
				Print("Error #");
				Print(temp);
			}
		}
	}

	if (_outFileStream) {
		if (_processedFileInfo.MTimeDefined)
			_outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
		RINOK(_outFileStreamSpec->Close());
	}
	_outFileStream.Release();
	if (_extractMode && _processedFileInfo.AttribDefined)
		SetFileAttrib_PosixHighDetect(_diskFilePath, _processedFileInfo.Attrib);
	PrintNewLine();
	return S_OK;
}


STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR* password)
{
	if (!PasswordIsDefined) {
		// You can ask real password here from user
		// Password = GetPassword(OutStream);
		// PasswordIsDefined = true;
		PrintError("Password is not defined");
		return E_ABORT;
	}
	return StringToBstr(Password, password);
}
