#include <translations.hpp>

Translations::CPhrase::CPhrase()
 :  m_map(DefLessFunc(const CKey_t))
{
}

const Translations::CPhrase::CFormat &Translations::CPhrase::GetFormat() const
{
	return m_aFormat;
}

const char *Translations::CPhrase::ParseFormatString(const char *psz, CBufferStringVector &vecMessages)
{
	return m_aFormat.ParseString(psz, vecMessages);
}

void Translations::CPhrase::InsertContent(const CKey_t nKey, const CContent &aData)
{
	m_map.Insert(nKey, aData);
}

bool Translations::CPhrase::Find(const CKey &sKey, const CContent *&psOutput) const
{
	auto iFound = m_map.Find((CKey_t)sKey);

	bool bResult = iFound != m_map.InvalidIndex();

	if(bResult)
	{
		psOutput = &m_map.Element(iFound);
	}

	return bResult;
}
