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

Translations::Translations()
 :  m_mapPhrases(DefLessFunc(const CUtlSymbolLarge))
{
}

Translations::CKey_t Translations::GetKeyT(const char *pszInit)
{
	CKey_t nResult;

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
		nResult = *(uint16 *)pszInit;
	}
	else if(!pszInit[3])
	{
		nResult = *(uint16 *)pszInit << 8 | *pszInit;
	}
	else
	{
		AssertMsg(0, "Translations key over 4 characters");
	}

	return nResult;
}

Translations::CKey::CKey(const char *pszInit)
{
	*(CKey_t *)this = GetKeyT(pszInit);
}

Translations::CKey::CKey(const CKey_t nInit)
{
	*(CKey_t *)this = nInit;
}

Translations::CKey_t Translations::CKey::Get() const
{
	return *(CKey_t *)m_sCode;
}

const char *Translations::CKey::String() const
{
	return m_sCode;
}

bool Translations::CKey::operator==(const CKey &aRigth) const
{
	return Get() == aRigth.Get();
}

bool Translations::CKey::operator!=(const CKey &aRigth) const
{
	return Get() != aRigth.Get();
}

bool Translations::CKey::operator<(const CKey &aRigth) const
{
	return Get() < aRigth.Get();
}

Translations::CPhrase::CPhrase()
 :  m_map(DefLessFunc(const CKey_t))
{
}

CUtlString Translations::CPhrase::CContent::Format(const CFormat &aData, size_t nCount, ...)
{
	const auto &mapFrames = aData.GetFrames();

	CUtlString sResult = Get();

	{
		va_list aParams;

		va_start(aParams, nCount);

		for(size_t n = 1; n <= nCount; n++)
		{
			auto iFound = mapFrames.Find(n);

			if(iFound == mapFrames.InvalidIndex())
			{
				continue;
			}

			CBufferStringGrowable<MAX_TRANSLATIONS_FORMAT_FRAME_TARGET_LENGTH> sFrameTarget;

			sFrameTarget.Format("{%zd}", n);

			auto aFrame = mapFrames.Element(iFound);

			CBufferStringGrowable<MAX_TRANSLATIONS_FORMAT_FRAME_RESULT_LENGTH> sFrameResult;

			switch(*aFrame.GetArgument())
			{
				case 'b':
				{
					auto nValue = va_arg(aParams, int);

					do
					{
						if(nValue & 1)
						{
							sFrameResult.AppendRepeat('1', 1);
						}
						else
						{
							sFrameResult.AppendRepeat('0', 1);
						}

						nValue >>= 1;
					}
					while(nValue);

					break;
				}

				case 'c':
				{
					sFrameResult.AppendRepeat((char)va_arg(aParams, int), 1);

					break;
				}

				case 'd':
				case 'i':
				{
					sFrameResult.Format("%d", va_arg(aParams, int));

					break;
				}

				case 'u':
				{
					sFrameResult.Format("%u", va_arg(aParams, uint));

					break;
				}

				case 'f':
				{
					sFrameResult.Format("%lf", va_arg(aParams, double));

					break;
				}

				case 's':
				{
					const char *pszConcat[] = {va_arg(aParams, const char *)};

					sFrameResult.AppendConcat(ARRAYSIZE(pszConcat), pszConcat, NULL);

					break;
				}

				case 'x':
				{
					sFrameResult.Format("%x", va_arg(aParams, int));

					break;
				}

				case 'X':
				{
					sFrameResult.Format("%X", va_arg(aParams, int));

					break;
				}
			}

			sResult.Replace(sFrameTarget.Get(), sFrameResult.Get());
		}

		va_end(aParams);
	}

	return sResult;
}

Translations::CPhrase::CFormat::CFormat()
 :  m_mapFrames(DefLessFunc(const CFrame_t))
{
}

Translations::CPhrase::CFormat::CFrame::CFrame()
 :  m_sArgument("")
{
}

const Translations::CPhrase::CFormat &Translations::CPhrase::GetFormat() const
{
	return m_aFormat;
}

bool Translations::CPhrase::Find(const CKey &sKey, CContent &sOutput) const
{
	auto iFound = m_map.Find((CKey_t)sKey);

	bool bResult = iFound != m_map.InvalidIndex();

	if(bResult)
	{
		sOutput = m_map.Element(iFound);
	}

	return bResult;
}

const char *Translations::CPhrase::ParseFormatString(const char *psz, CBufferStringVector &vecMessages)
{
	return m_aFormat.ParseString(psz, vecMessages);
}

void Translations::CPhrase::InsertContent(const CKey_t nKey, const CContent &aData)
{
	m_map.Insert(nKey, aData);
}

const char *Translations::CPhrase::CFormat::CFrame::GetArgument() const
{
	return m_sArgument;
}

const char *Translations::CPhrase::CFormat::CFrame::ParseString(const char *psz, CBufferStringVector &vecMessages)
{
	size_t nLength = 0;

	while(true)
	{
		if(*psz)
		{
			if(*psz == '}')
			{
				m_sArgument[nLength] = '\0';

				return psz;
			}
			else if(nLength < sizeof(m_sArgument) - 1)
			{
				m_sArgument[nLength++] = *psz;
			}
		}
		else
		{
			m_sArgument[nLength] = '\0';

			static const char *s_pszMessageConcat[] = {"Format: ", "no end"};

			vecMessages.AddToTail(s_pszMessageConcat);

			return psz;
		}

		psz++;
	}

	return psz;
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

bool Translations::FindPhrase(const char *pszName, int &iFound) const
{
	iFound = m_mapPhrases.Find(FindPhraseSymbol(pszName));

	return iFound != m_mapPhrases.InvalidIndex();
}

const Translations::CPhrase &Translations::GetPhrase(int iFound) const
{
	return m_mapPhrases.Element(iFound);
}

bool Translations::Parse(const KeyValues3 *pRoot, CBufferStringVector &vecMessages)
{
	int iMemberCount = pRoot->GetMemberCount();

	if(!iMemberCount)
	{
		static const char *s_pszMessageConcat[] = {"No members"};

		vecMessages.AddToTail(s_pszMessageConcat);

		return true;
	}

	for(KV3MemberId_t n = 0; n < iMemberCount; n++)
	{
		const char *pszPhraseName = pRoot->GetMemberName(n);

		const KeyValues3 *pPhraseDataKeys = pRoot->GetMember(n);

		ParsePhrase(pszPhraseName, pPhraseDataKeys, vecMessages);
	}

	return true;
}

bool Translations::ParsePhrase(const char *pszName, const KeyValues3 *pDataKeys, CBufferStringVector &vecMessages)
{
	int iMemberCount = pDataKeys->GetMemberCount();

	if(!iMemberCount)
	{
		static const char *s_pszMessageConcat[] = {"No members"};

		vecMessages.AddToTail(s_pszMessageConcat);

		return false;
	}

	CPhrase aPhrase {};

	for(KV3MemberId_t n = 0; n < iMemberCount; n++)
	{
		const char *pszKey = pDataKeys->GetMemberName(n);

		const char *pszValue = pDataKeys->GetMember(n)->GetString();

		if(!V_strcmp(pszKey, "#format"))
		{
			aPhrase.ParseFormatString(pszValue, vecMessages);
		}
		else
		{
			aPhrase.InsertContent(GetKeyT(pszKey), pszValue);
		}
	}

	m_mapPhrases.Insert(GetPhraseSymbol(pszName), aPhrase);

	return true;
}

void Translations::Purge()
{
	m_mapPhrases.Purge();
	m_aPhraseSymbolTable.Purge();
}

CUtlSymbolLarge Translations::GetPhraseSymbol(const char *pszName)
{
	return m_aPhraseSymbolTable.AddString(pszName);
}

CUtlSymbolLarge Translations::FindPhraseSymbol(const char *pszName) const
{
	return m_aPhraseSymbolTable.Find(pszName);
}
