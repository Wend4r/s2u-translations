#include <translations.hpp>

const char *Translations::CPhrase::CFormat::CFrame::ParseString(const char *pszText, CStringVector &vecMessages)
{
	size_t nLength = 0;

	while(true)
	{
		if(*pszText)
		{
			if(*pszText == '}')
			{
				m_sArgument[nLength] = '\0';

				return pszText + 1;
			}
			else if(nLength < sizeof(m_sArgument) - 1)
			{
				m_sArgument[nLength++] = *pszText;
			}
		}
		else
		{
			m_sArgument[nLength] = '\0';

			vecMessages.AddToTail(CSmallBufferString({"Format: ", "no end"}));

			return pszText;
		}

		pszText++;
	}

	return pszText;
}
