#include <translations.hpp>

Translations::CPhrase::CFormat::CFormat()
 :  m_mapFrames(DefLessFunc(const CFrame_t))
{
}

const CUtlMap<Translations::CPhrase::CFormat::CFrame_t, Translations::CPhrase::CFormat::CFrame> &Translations::CPhrase::CFormat::GetFrames() const
{
	return m_mapFrames;
}

CUtlString Translations::CPhrase::CFormat::GenerateString() const
{
	CBufferStringGrowable<1024> sResult;

	FOR_EACH_MAP_FAST(m_mapFrames, iFrame)
	{
		sResult.AppendFormat("{%d:%s},", m_mapFrames.Key(iFrame), m_mapFrames.Element(iFrame).GetArgument());
	}

	sResult.SetLength(sResult.GetTotalNumber() - 1);

	return sResult;
}

const char *Translations::CPhrase::CFormat::ParseString(const char *psz, CBufferStringVector &vecMessages)
{
	do
	{
		CFrame_t nFrame {};

		CFrame aData {};

		while(*psz)
		{
			if(*psz == ',')
			{
				psz++;
			}

			if(*psz == '{')
			{
				psz++;
			}
			else
			{
				static const char *s_pszMessageConcat[] = {"Format: ", "no start with"};

				vecMessages.AddToTail(s_pszMessageConcat);

				return psz;
			}

			{
				char *psNextIterfator;

				nFrame = (CFormat_t)strtoul(psz, &psNextIterfator, 10);
				psz = psNextIterfator;
			}

			if(*psz == ':')
			{
				psz++;
			}
			else
			{
				static const char *s_pszMessageConcat[] = {"Format: ", "no separator character"};

				vecMessages.AddToTail(s_pszMessageConcat);

				return psz;
			}

			psz = aData.ParseString(psz, vecMessages);
		}

		m_mapFrames.Insert(nFrame, aData);
	}
	while(*psz && *psz == ',');

	return psz;
}