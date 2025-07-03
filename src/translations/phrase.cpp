#include <translations.hpp>

bool Translations::CPhrase::Find(const CKey &sKey, const CContent *&psOutput) const
{
	auto iFound = m_map.Find(static_cast<Key_t>(sKey));

	bool bResult = iFound != m_map.InvalidIndex();

	if(bResult)
	{
		psOutput = &m_map.Element(iFound);
	}

	return bResult;
}
