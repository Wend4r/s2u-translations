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

#include <stdarg.h>

#include <tier0/bufferstring.h>
#include <tier0/platform.h>
#include <tier0/strtools.h>
#include <tier0/utlsymbol.h>
#include <tier1/utlmap.h>
#include <tier1/utlsymbollarge.h>

#define MAX_TRANSLATIONS_PHRASE_LENGTH 512
#define MAX_TRANSLATIONS_FORMAT_FRAME_TARGET_LENGTH 32 // 2 ("{}") + 11 ("-2147483648") + 1 (null terminated) = 14
#define MAX_TRANSLATIONS_FORMAT_FRAME_RESULT_LENGTH 512

class KeyValues3;

class Translations
{
public:
	using CStringVector = CUtlVector<CUtlString>;

	using CPhraseBuffer = CBufferStringN<MAX_TRANSLATIONS_PHRASE_LENGTH>;
	using CFormatBuffer = CBufferStringN<MAX_TRANSLATIONS_FORMAT_FRAME_TARGET_LENGTH>;
	using CFrameBuffer = CBufferStringN<MAX_TRANSLATIONS_FORMAT_FRAME_RESULT_LENGTH>;

public:
	Translations() : m_mapPhrases(DefLessFunc(const CUtlSymbolLarge)) {}

public:
	using Key_t = uint64;
	static Key_t Key(const char *pszInit);

	class CKey
	{
	public:
		CKey(const char *pszInit) : m_nKey(Key(pszInit)) {}
		CKey(const Key_t nInit) : m_nKey(nInit) {}
		Key_t Get() const { return m_nKey; }
		const char *String() const { return m_sCode; }

		operator Key_t() const { return Get(); }
		bool operator==(const CKey &aRigth) const { return Get() == aRigth.Get(); }
		bool operator!=(const CKey &aRigth) const { return !operator==(aRigth); }
		bool operator<(const CKey &aRigth) const { return Get() < aRigth.Get(); }

	private:
		union
		{
			Key_t m_nKey;
			char m_sCode[sizeof(Key_t)];
		};
	}; // CKey

	class CPhrase
	{
		friend class Translations;

	public:
		CPhrase(CUtlSymbolTable *pTable = nullptr) : m_aFormat(pTable), m_map(DefLessFunc(const Key_t)) {}
		CPhrase(const CPhrase &copyFrom) { CopyFrom(copyFrom); }
		CPhrase(CPhrase &&moveFrom) { MoveFrom(Move(moveFrom)); }
		CPhrase &operator=(const CPhrase &copyFrom) { return CopyFrom(copyFrom); }
		CPhrase &operator=(CPhrase &&moveFrom) { return MoveFrom(Move(moveFrom)); }

		CPhrase &CopyFrom(const CPhrase &copyFrom)
		{
			m_aFormat = copyFrom.m_aFormat;
			m_map = copyFrom.m_map;

			return *this;
		}
		CPhrase &MoveFrom(CPhrase &&moveFrom)
		{
			m_aFormat = Move(moveFrom.m_aFormat);
			m_map = Move(moveFrom.m_map);

			return *this;
		}

	public:
		class CFormat;

		class CContent : public CUtlString
		{
		public:
			using CUtlString::CUtlString;

		public:
			CUtlString FormatV(const CFormat &aData, va_list aParams) const;
			CUtlString Format(const CFormat &aData, int nCount, ...) const;
		}; // CContent

		class CFormat
		{
			friend class CPhrase;

		public:
			struct Argument_t
			{
				CUtlSymbol symName;
				CBufferStringN<8> sFormatMark;
			};

			CFormat(CUtlSymbolTable *pTable = nullptr) : m_vecArgs(0, 8), m_pTable(pTable) {}

		public:
			const CUtlSymbolTable *Table() const { return m_pTable; }
			const CUtlVector<Argument_t> &Args() const { return m_vecArgs; }
			CUtlString GenerateString() const;

		protected:
			const char *ParseString(const char *pszText, CStringVector &vecMessages);

		private:
			CUtlVector<Argument_t> m_vecArgs;
			CUtlSymbolTable *m_pTable;
		}; // CFormat

		const CFormat &GetFormat() const { return m_aFormat; }
		bool Find(const CKey &sCountryCode, const CContent *&psOutput) const;

	protected:
		const char *ParseFormatString(const char *psz, CStringVector &vecMessages) { return m_aFormat.ParseString(psz, vecMessages); }
		void InsertContent(const Key_t nKey, CContent &&aData) { m_map.Insert(nKey, Move(aData)); }

	private:
		CFormat m_aFormat;
		CUtlMap<Key_t, CContent> m_map;
	}; // CPhrase

public:
	bool FindPhrase(const char *pszName, int &iFound) const;
	const CPhrase &GetPhrase(int iFound) const { return m_mapPhrases.Element(iFound); }

public:
	using CPhraseContent = CPhrase::CContent;

	class IPhraseReplacer
	{
	public:
		virtual CUtlString ProcessText(const CUtlString &sPhrase) const = 0;
	};

	bool Parse(const KeyValues3 *pRoot, IPhraseReplacer *pReplacer, CStringVector &vecMessages);
	bool ParsePhrase(const char *pszName, const KeyValues3 *pDataKeys, IPhraseReplacer *pReplacer, CStringVector &vecMessages);

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
	CUtlSymbolTable m_tableFormatMarks;
	CUtlSymbolTableLarge_CI m_aPhraseSymbolTable;
	CUtlMap<CUtlSymbolLarge, CPhrase> m_mapPhrases;
}; // Translations

#endif // _INCLUDE_TRANSLATIONS_HPP_
