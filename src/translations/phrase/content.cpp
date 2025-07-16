#include <translations.hpp>

#include <stddef.h>
#include <string.h>

CUtlString Translations::CPhrase::CContent::FormatV(const CFormat &aData, va_list aParams) const
{
	const auto *pTable = aData.Table();

	AssertMsg(pTable, "Format marks table is not ready");

	const auto &vecArgs = aData.Args();

	CPhraseBuffer sResult = static_cast<CUtlString>(*this);

	for(const auto &arg : vecArgs)
	{
		CFormatBuffer sFrameTarget;
		CFrameBuffer sFrameResult;

		sFrameTarget.Format("{%s}", pTable->String(arg.symName));

		const char *pszFormatArg = arg.sFormatMark.String();

		switch(*pszFormatArg)
		{
			case 'b':
			{
				auto nValue = va_arg(aParams, int);

				do
				{
					sFrameResult += (nValue & 1) ? '1' : '0';
					nValue >>= 1;
				}
				while(nValue);

				break;
			}

			case 'c':
			{
				sFrameResult += static_cast<char>(va_arg(aParams, int));

				break;
			}

			case 's':
			{
				const char *pszConcat[] = {va_arg(aParams, const char *)};

				sFrameResult.AppendConcatN(pszConcat);

				break;
			}

			default:
			{
				char sFormat[8] = "%";

				strncpy(&sFormat[1], pszFormatArg, sizeof(sFormat) - 1);
				sFrameResult.AppendFormatV(sFormat, aParams);

				// Skip va argument to next FormatV.
				{
					bool bIsFloatPoint = false;

					char cSpecifier;

					while((cSpecifier = *pszFormatArg) && !bIsFloatPoint)
					{
						switch(cSpecifier | (1 << 5))
						{
							case 'a':
							case 'e':
							case 'f':
							case 'g':
							{
								bIsFloatPoint = true;

								break;
							}
						}

						pszFormatArg++;
					}

					if(bIsFloatPoint)
					{
						va_arg(aParams, double);
					}
					else
					{
						va_arg(aParams, int);
					}
				}

				break;
			}
		}

		sResult.Replace(sFrameTarget.String(), sFrameResult.String());
	}

	return sResult;
}


CUtlString Translations::CPhrase::CContent::Format(const CFormat &aData, int nCount, ...) const
{
	va_list aParams;

	va_start(aParams, nCount);

	auto sResult = FormatV(aData, aParams);

	va_end(aParams);

	return sResult;
}
