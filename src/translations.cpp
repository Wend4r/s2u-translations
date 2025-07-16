/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * Tranlation util for Source2 games.
 * Written by Wend4r (2024).
 * ======================================================

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <translations.hpp>

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <tier0/commonmacros.h>
#include <tier1/keyvalues3.h>

Translations::Key_t Translations::Key(const char *pszInit)
{
	Key_t nResult {};

	if(!pszInit[0])
	{
		nResult = 0;
	}
	else if(!pszInit[1])
	{
		nResult = pszInit[0];
	}
	else if(!pszInit[2])
	{
		nResult = *reinterpret_cast<const uint16 *>(pszInit);
	}
	else if(!pszInit[3])
	{
		nResult = *reinterpret_cast<const uint16 *>(pszInit + 1) << 8 | *pszInit;
	}
	else if(!pszInit[4])
	{
		nResult = *reinterpret_cast<const uint32 *>(pszInit);
	}
	else if (!pszInit[5])
	{
		nResult = (static_cast<uint64>(*reinterpret_cast<const uint32 *>(pszInit))) |
		          (static_cast<uint64>(pszInit[4]) << 32);
	}
	else if (!pszInit[6])
	{
		nResult = (static_cast<uint64>(*reinterpret_cast<const uint32 *>(pszInit))) |
		          (static_cast<uint64>(*reinterpret_cast<const uint16 *>(pszInit + 4)) << 32);
	}
	else if (!pszInit[7])
	{
		nResult = (static_cast<uint64>(*reinterpret_cast<const uint32 *>(pszInit))) |
		          (static_cast<uint64>(*reinterpret_cast<const uint16 *>(pszInit + 4)) << 32) |
		          (static_cast<uint64>(pszInit[6]) << 48);
	}
	else
	{
		AssertMsg(0, "Translations key over 7 characters");
	}

	return nResult;
}

bool Translations::FindPhrase(const char *pszName, int &iFound) const
{
	iFound = m_mapPhrases.Find(FindPhraseSymbol(pszName));

	return iFound != m_mapPhrases.InvalidIndex();
}

bool Translations::Parse(const KeyValues3 *pRoot, IPhraseReplacer *pReplacer, CStringVector &vecMessages)
{
	// Entering to "Phrases" (if any).
	{
		const KeyValues3 *pPhrasesData = pRoot->FindMember("Phrases");

		if(pPhrasesData)
		{
			pRoot = pPhrasesData;
		}
	}

	int iMemberCount = pRoot->GetMemberCount();

	if(!iMemberCount)
	{
		vecMessages.AddToTail("No members");

		return true;
	}

	KV3MemberId_t n = 0;

	do
	{
		ParsePhrase(pRoot->GetMemberName(n), pRoot->GetMember(n), pReplacer, vecMessages);
		n++;
	}
	while(n < iMemberCount);

	return true;
}

bool Translations::ParsePhrase(const char *pszName, const KeyValues3 *pDataKeys, IPhraseReplacer *pReplacer, CStringVector &vecMessages)
{
	int iMemberCount = pDataKeys->GetMemberCount();

	if(!iMemberCount)
	{
		vecMessages.AddToTail("No members");

		return false;
	}

	CUtlSymbolLarge sPhrase = GetPhraseSymbol(pszName);

	auto &mapPhrases = m_mapPhrases;

	auto iPhraseKey = mapPhrases.InsertOrReplace(sPhrase, CPhrase(&m_tableFormatMarks));

	auto &aPhrase = mapPhrases.Element(iPhraseKey);

	KV3MemberId_t n = 0;

	do
	{
		const char *pszKey = pDataKeys->GetMemberName(n);

		const KeyValues3 *pKV = pDataKeys->GetMember(n);

		if(!pKV)
		{
			n++;

			continue;
		}

		const char *pszValue = pKV->GetString();

		if(!V_strcmp(pszKey, "#format"))
		{
			aPhrase.ParseFormatString(pszValue, vecMessages);
		}
		else
		{
			CUtlString sPhrase(pszValue);

			sPhrase = pReplacer->ProcessText(sPhrase);
			aPhrase.InsertContent(Key(pszKey), CPhraseContent(sPhrase.String()));
		}

		n++;
	}
	while(n < iMemberCount);
	
	return true;
}
