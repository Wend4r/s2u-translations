#include <translations.hpp>

CUtlString Translations::CPhrase::CFormat::GenerateString() const
{
	CXLargeBufferString sResult;

	for(const auto &arg : m_vecArgs)
	{
		sResult.AppendFormat("{%s:%s},", Table()->String(arg.symName), arg.sFormatMark.String());
	}

	sResult.SetLength(sResult.Length() - 1);

	return sResult;
}

const char *Translations::CPhrase::CFormat::ParseString(const char *pszText, CStringVector &vecMessages)
{
	const char *pszFirstMark = nullptr;

	while(*pszText)
	{
		if(*pszText == '{')
		{
			pszText++;
			pszFirstMark = pszText;
		}

		if(*pszText == ':')
		{
			if(!pszFirstMark)
			{
				vecMessages.AddToTail(CSmallBufferString({"Format: ", "no formated mark"}));

				break;
			}

			CSmallBufferString sFormatMark(pszFirstMark, static_cast<int>(pszText - pszFirstMark));

			pszText++;

			CBufferStringN<8> sFormatArg;

			while(*pszText != '}')
			{
				sFormatArg += *pszText;
				pszText++;

				if(!*pszText)
				{
					vecMessages.AddToTail(CSmallBufferString({"Format: ", "no closed curly brace"}));

					return pszText;
				}
			}

			m_vecArgs.AddToTail(Argument_t{m_pTable->AddString(sFormatMark.String()), Move(sFormatArg)});

			pszFirstMark = nullptr;
		}

		pszText++;
	}

	return pszText;
}
