/**
 * vim: set ts=4 sw=4 tw=99 noet :
 * ======================================================
 * Translationss util for Source2 games.
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

#ifndef _INCLUDE_TRANSLATIONS_HPP_
#define _INCLUDE_TRANSLATIONS_HPP_

#include <tier0/bufferstring.h>
#include <tier0/platform.h>
#include <tier0/strtools.h>
#include <tier1/utlmap.h>
#include <tier1/utlsymbollarge.h>

#define MAX_TRANSLATIONS_MESSAGE_LENGTH 256

#define MAX_TRANSLATIONS_FORMAT_FRAME_TARGET_LENGTH 32 // 2 ("{}") + 11 ("-2147483648") + 1 (null terminated) = 14
#define MAX_TRANSLATIONS_FORMAT_FRAME_RESULT_LENGTH 512

class KeyValues3;

class Translations
{
public:
	using CStringVector = CUtlVector<CUtlString>;

public:
	Translations();

public:
	using Key_t = uint32;
	static Key_t GetKey(const char *pszInit);

	class CKey
	{
	public:
		CKey(const char *pszInit) { *reinterpret_cast<Key_t *>(this) = GetKey(pszInit); }
		CKey(const Key_t nInit) { *reinterpret_cast<Key_t *>(this) = nInit; }
		Key_t Get() const { return *reinterpret_cast<const Key_t *>(m_sCode); }
		const char *String() const { return m_sCode; }

		operator Key_t() const { return Get(); }
		bool operator==(const CKey &aRigth) const { return Get() == aRigth.Get(); }
		bool operator!=(const CKey &aRigth) const { return !operator==(aRigth); }
		bool operator<(const CKey &aRigth) const { return Get() < aRigth.Get(); }

	private:
		char m_sCode[4];
	}; // CKey

	class CPhrase
	{
		friend class Translations;

	public:
		CPhrase() : m_map(DefLessFunc(const Key_t)) {}

	public:
		class CFormat;

		using CContentBase = CUtlString;

		class CContent : public CContentBase
		{
		public:
			using CBase = CContentBase;
			using CBase::CBase;

		public:
			CUtlString Format(const CFormat &aData, size_t nCount, ...) const;
		}; // CContent

		class CFormat
		{
			friend class CPhrase;

		public:
			CFormat() : m_mapFrames(DefLessFunc(const CFrame_t)) {}

			using CFrame_t = uint32;

			class CFrame
			{
				friend class CFormat;

			public:
				const char *GetArgument() const { return m_sArgument; }

			protected:
				const char *ParseString(const char *pszText, CStringVector &vecMessages);

			private:
				char m_sArgument[8];
			}; // CFrame

		public:
			const CUtlMap<CFrame_t, CFrame> &GetFrames() const { return m_mapFrames; }
			CUtlString GenerateString() const;

		protected:
			const char *ParseString(const char *pszText, CStringVector &vecMessages);

		private:
			CUtlMap<CFrame_t, CFrame> m_mapFrames;
		}; // CFormat

		const CFormat &GetFormat() const { return m_aFormat; }
		bool Find(const CKey &sCountryCode, const CContent *&psOutput) const;

	protected:
		const char *ParseFormatString(const char *psz, CStringVector &vecMessages) { return m_aFormat.ParseString(psz, vecMessages); }
		void InsertContent(const Key_t nKey, const CContent &aData) { m_map.Insert(nKey, aData); }

	private:
		using CFormat_t = uint32;

		CFormat m_aFormat;
		CUtlMap<Key_t, CContent> m_map;
	}; // CPhrase

public:
	bool FindPhrase(const char *pszName, int &iFound) const;
	const CPhrase &GetPhrase(int iFound) const { return m_mapPhrases.Element(iFound); }

public:
	bool Parse(const KeyValues3 *pRoot, CStringVector &vecMessages);
	bool ParsePhrase(const char *pszName, const KeyValues3 *pDataKeys, CStringVector &vecMessages);

public:
	void Purge()
	{
		m_mapPhrases.Purge();
		m_aPhraseSymbolTable.Purge();
	}

protected:
	CUtlSymbolLarge GetPhraseSymbol(const char *pszName) { return m_aPhraseSymbolTable.AddString(pszName); }
	CUtlSymbolLarge FindPhraseSymbol(const char *pszName) const { return m_aPhraseSymbolTable.Find(pszName); }

private:
	CUtlSymbolTableLarge_CI m_aPhraseSymbolTable;
	CUtlMap<CUtlSymbolLarge, CPhrase> m_mapPhrases;
}; // Translations

#endif // _INCLUDE_TRANSLATIONS_HPP_
