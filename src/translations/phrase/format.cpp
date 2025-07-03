#include <translations.hpp>

CUtlString Translations::CPhrase::CFormat::GenerateString() const
{
	CXLargeBufferString sResult;

	FOR_EACH_MAP_FAST(m_mapFrames, iFrame)
	{
		sResult.AppendFormat("{%d:%s},", m_mapFrames.Key(iFrame), m_mapFrames.Element(iFrame).GetArgument());
	}

	sResult.SetLength(sResult.Length() - 1);

	return sResult;
}

const char *Translations::CPhrase::CFormat::ParseString(const char *pszText, CStringVector &vecMessages)
{
	do
	{
		const auto &iInvalid = decltype(m_mapFrames)::InvalidIndex();

		decltype(m_mapFrames)::KeyType_t iKey = iInvalid;

		while(*pszText)
		{
			if(*pszText == ',')
			{
				pszText++;
			}

			if(*pszText == '{')
			{
				pszText++;
			}
			else
			{
				vecMessages.AddToTail(CSmallBufferString({"Format: ", "no start with"}));

				return pszText;
			}

			{
				char *psNextIterfator;

				iKey = m_mapFrames.Insert(static_cast<CFormat_t>(strtoul(pszText, &psNextIterfator, 10)));
				pszText = psNextIterfator;
			}

			if(*pszText == ':')
			{
				pszText++;
			}
			else
			{
				vecMessages.AddToTail(CSmallBufferString({"Format: ", "no separator character"}));

				return pszText;
			}

			if(iKey != iInvalid)
			{
				pszText = m_mapFrames.Element(iKey).ParseString(pszText, vecMessages);
			}
		}
	}
	while(*pszText && *pszText == ',');

	return pszText;
}
