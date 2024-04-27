#include "stdafx.h"
#include "ADOUtils.h"

void ADOErrorHandler::HandlerError(_com_error& e)
{
    FILE_LOG("===============COM ERROR================");
    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
    FILE_LOG("Error\n");
    FILE_LOG("\tCode = %08lx\n", e.Error());
    FILE_LOG(L"\tCode meaning = %s\n", e.ErrorMessage());
    FILE_LOG("\tSource = %s\n", (LPCSTR)bstrSource);
    FILE_LOG("\tDescription = %s\n", (LPCSTR)bstrDescription);
}