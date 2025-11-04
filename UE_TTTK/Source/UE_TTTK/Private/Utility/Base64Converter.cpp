// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/Base64Converter.h"


FString UBase64Converter::StringConvertToBase64(const FString& InString)
{
	const FTCHARToUTF8 utf8Array(*InString);
	TArray<const uint8> unsignedCharArray(reinterpret_cast<const uint8*>(utf8Array.Get()), utf8Array.Length());
	
	return FBase64::Encode(unsignedCharArray.GetData(), unsignedCharArray.Num());
}

FString UBase64Converter::StringConvertFromBase64(const FString& InString)
{
	TArray<uint8> decoded;
	FBase64::Decode(InString, decoded);
	TArray<const UTF8CHAR> utf8Array(reinterpret_cast<const UTF8CHAR*>(decoded.GetData()), decoded.Num());
	return FString(UTF8_TO_TCHAR(utf8Array.GetData()));
}
