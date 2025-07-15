#include <translations.hpp>

CUtlString Translations::CPhrase::CFormat::GenerateString() const
{
	CXLargeBufferString sResult;

	FOR_EACH_MAP_FAST(m_mapFormat, iFrame)
	{
		sResult.AppendFormat("{%d:%s},", m_mapFormat.Key(iFrame).GetId(), m_mapFormat.Element(iFrame).String());
	}

	sResult.SetLength(sResult.Length() - 1);

	return sResult;
}

const char *Translations::CPhrase::CFormat::ParseString(const char *pszText, CStringVector &vecMessages)
{
	const char *pszFirstMark = nullptr;

	const auto iInvalid = decltype(m_mapFormat)::InvalidIndex();

	decltype(m_mapFormat)::KeyType_t iKey = iInvalid;

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

				return pszText;
			}

			CSmallBufferString sFormatMark(pszFirstMark, static_cast<int>(pszText - pszFirstMark));

			iKey = m_mapFormat.Insert(m_pTable->AddString(sFormatMark.String()));

			pszText++;
			pszFirstMark = nullptr;
		}

		if(iKey != iInvalid)
		{
			CBufferString sFormatArg;

			while(*pszText && *pszText != '}')
			{
				sFormatArg += *pszText;
				pszText++;
			}

			m_mapFormat.Element(iKey) = Move(sFormatArg);
			iKey = iInvalid;
		}

		if(*pszText == ',')
		{
			iKey = iInvalid;
		}

		pszText++;
	}

	return pszText;
}
