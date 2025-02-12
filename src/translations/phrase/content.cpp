#include <translations.hpp>

CUtlString Translations::CPhrase::CContent::Format(const CFormat &aData, size_t nCount, ...) const
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

			CBufferStringN<MAX_TRANSLATIONS_FORMAT_FRAME_TARGET_LENGTH> sFrameTarget;

			sFrameTarget.Format("{%zd}", n);

			auto aFrame = mapFrames.Element(iFound);

			CBufferStringN<MAX_TRANSLATIONS_FORMAT_FRAME_RESULT_LENGTH> sFrameResult;

			const char *pszFormatType = aFrame.GetArgument();

			switch(*pszFormatType)
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
					sFrameResult.AppendRepeat(static_cast<char>(va_arg(aParams, int)), 1);

					break;
				}

				case 's':
				{
					const char *pszConcat[] = {va_arg(aParams, const char *)};

					sFrameResult.AppendConcat(ARRAYSIZE(pszConcat), pszConcat, NULL);

					break;
				}

				default:
				{
					char sFormat[8] = "%";

					strncpy(&sFormat[1], pszFormatType, sizeof(sFormat) - 1);
					sFrameResult.AppendFormatV(sFormat, aParams);

					break;
				}
			}

			sResult = sResult.Replace(sFrameTarget.Get(), sFrameResult.Get());
		}

		va_end(aParams);
	}

	return sResult;
}
