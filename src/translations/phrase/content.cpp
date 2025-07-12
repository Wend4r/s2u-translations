#include <translations.hpp>

#include <stddef.h>
#include <string.h>

CUtlString Translations::CPhrase::CContent::FormatV(const CFormat &aData, va_list aParams) const
{
	const auto &mapFrames = aData.GetFrames();

	CPhraseBuffer sResult = static_cast<CUtlString>(*this);

	{
		for(size_t n = 1, nCount = mapFrames.Count(); n <= nCount; n++)
		{
			auto iFound = mapFrames.Find(n);

			if(iFound == mapFrames.InvalidIndex())
			{
				continue;
			}

			CFormatBuffer sFrameTarget;
			CFrameBuffer sFrameResult;

			sFrameTarget.Format("{%zd}", n);

			const auto &aFrame = mapFrames.Element(iFound);

			const char *pszFormatType = aFrame.GetArgument();

			switch(*pszFormatType)
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

					strncpy(&sFormat[1], pszFormatType, sizeof(sFormat) - 1);
					sFrameResult.AppendFormatV(sFormat, aParams);

					// Skip va argument to next FormatV.
					{
						bool bIsFloatPoint = false;

						char cSpecifier;

						while((cSpecifier = *pszFormatType) && !bIsFloatPoint)
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

							pszFormatType++;
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

			sResult.Replace(sFrameTarget.Get(), sFrameResult.Get());
		}
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
