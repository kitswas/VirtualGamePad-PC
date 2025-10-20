# Standalone CMake script to create an MSIX package for VirtualGamePad-PC.
# This script is intended to be executed with: cmake -DAPP_VERSION=... -DMSIX_PFX_FILE=... -P create_msix.cmake

message(STATUS "Starting MSIX creation script (standalone)")

# Determine project source dir (script is in <source>/cmake)
get_filename_component(SCRIPT_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
get_filename_component(SOURCE_DIR "${SCRIPT_DIR}/.." ABSOLUTE)
set(DIST_DIR "${SOURCE_DIR}/dist")
file(MAKE_DIRECTORY "${DIST_DIR}")

# Convert APP_VERSION into a 4-part numeric version string for AppxManifest
# Split on dots into a CMake list, clean non-digits, pad with zeros or trim to exactly 4 parts
string(REPLACE "." ";" _version_parts "${APP_VERSION}")
set(_parts_clean)
foreach(_p IN LISTS _version_parts)
	string(REGEX REPLACE "[^0-9]" "" _p_clean "${_p}")
	if(_p_clean STREQUAL "")
		set(_p_clean "0")
	endif()
	list(APPEND _parts_clean "${_p_clean}")
endforeach()
# Pad to 4
while(NOT "${_parts_clean}" STREQUAL "")
	list(LENGTH _parts_clean _len)
	if(_len GREATER_EQUAL 4)
		break()
	endif()
	list(APPEND _parts_clean "0")
endwhile()
# If still less than 4 (empty input), ensure we have 4 zeros
list(LENGTH _parts_clean _len2)
if(_len2 EQUAL 0)
	set(_parts_clean 0 0 0 0)
endif()
# If more than 4, trim
list(LENGTH _parts_clean _len3)
if(_len3 GREATER 4)
	list(GET _parts_clean 0 _p0)
	list(GET _parts_clean 1 _p1)
	list(GET _parts_clean 2 _p2)
	list(GET _parts_clean 3 _p3)
	set(_parts_clean ${_p0} ${_p1} ${_p2} ${_p3})
endif()

# Join back to dotted version
string(REPLACE ";" "." MANIFEST_VERSION "${_parts_clean}")


# Use local logo if present
set(LOCAL_LOGO "${SOURCE_DIR}/res/logos/SquareIcon.png")
if(EXISTS "${LOCAL_LOGO}")
	file(COPY "${LOCAL_LOGO}" DESTINATION "${DIST_DIR}")
	message(STATUS "Using local logo: ${LOCAL_LOGO}")
else()
	message(WARNING "Local logo not found at ${LOCAL_LOGO}; continuing without logo. Consider adding res/logos/SquareIcon.png")
endif()

# Write AppxManifest.xml using APP_VERSION
set(MANIFEST_CONTENT "<?xml version= \"1.0\" encoding= \"utf-8\"?>  <Package xmlns= \"http://schemas.microsoft.com/appx/manifest/foundation/windows10\"  xmlns:uap= \"http://schemas.microsoft.com/appx/manifest/uap/windows10\"  xmlns:rescap= \"http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities\">  <Identity Name= \"io.github.kitswas.VirtualGamePad\" Publisher= \"CN=kitswas\" Version= \"${MANIFEST_VERSION}\" />   <Properties>    <DisplayName>VirtualGamePad-PC</DisplayName>   <PublisherDisplayName>kitswas</PublisherDisplayName>    <Logo>SquareIcon.png</Logo>  </Properties>   <Dependencies>   <TargetDeviceFamily    Name= \"Windows.Desktop\" MinVersion= \"10.0.0.0\" MaxVersionTested= \"10.0.26200.0\" />  </Dependencies>   <Resources>   <Resource    Language= \"en-us\" />  </Resources>   <Capabilities>   <rescap:Capability Name= \"runFullTrust\" />   <rescap:Capability Name= \"inputInjectionBrokered\" />  </Capabilities>    <Applications>   <Application Id= \"App\" Executable= \"bin\\VGamepadPC.exe\" EntryPoint= \"Windows.FullTrustApplication\">    <uap:VisualElements     DisplayName= \"VirtualGamePad-PC\" Description= \"VirtualGamePad-PC\" BackgroundColor= \"transparent\" Square150x150Logo= \"SquareIcon.png\"     Square44x44Logo= \"SquareIcon.png\" />   </Application>  </Applications> </Package> \n")

file(WRITE "${DIST_DIR}/AppxManifest.xml" "${MANIFEST_CONTENT}")
message(STATUS "Wrote ${DIST_DIR}/AppxManifest.xml")

# Helper: expand common ProgramFiles env vars using cmd to avoid CMake parser issues
execute_process(COMMAND cmd /C "echo %ProgramFiles(x86)%" OUTPUT_VARIABLE _pf86_raw OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process(COMMAND cmd /C "echo %ProgramFiles%" OUTPUT_VARIABLE _pf_raw OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process(COMMAND cmd /C "echo %ProgramW6432%" OUTPUT_VARIABLE _pw6432_raw OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
string(REPLACE "\\" "/" PF86 "${_pf86_raw}")
string(REPLACE "\\" "/" PF "${_pf_raw}")
string(REPLACE "\\" "/" PW6432 "${_pw6432_raw}")

# Find makeappx.exe by scanning common Program Files roots recursively (prefer ProgramFiles(x86))
set(_makeappx_candidates)
if(PF86)
	file(GLOB_RECURSE _tmp_makeappx "${PF86}/*/makeappx.exe" ERROR_QUIET)
	if(_tmp_makeappx)
		list(APPEND _makeappx_candidates ${_tmp_makeappx})
	endif()
endif()
if(PF)
	file(GLOB_RECURSE _tmp_makeappx2 "${PF}/*/makeappx.exe" ERROR_QUIET)
	if(_tmp_makeappx2)
		list(APPEND _makeappx_candidates ${_tmp_makeappx2})
	endif()
endif()
if(_makeappx_candidates)
	list(SORT _makeappx_candidates)
	list(GET _makeappx_candidates 0 MAKEAPPX)
endif()
if(NOT MAKEAPPX)
	message(FATAL_ERROR "makeappx.exe not found on this machine. Please ensure Windows SDK or Visual Studio is installed.")
endif()
message(STATUS "Using MakeAppx: ${MAKEAPPX}")

# Pack MSIX
set(OUTPUT_MSIX "${DIST_DIR}/VirtualGamePadPC.msix")
# If output exists, try to remove it so makeappx can write
if(EXISTS "${OUTPUT_MSIX}")
	message(STATUS "Existing MSIX found: ${OUTPUT_MSIX} - attempting to remove before packaging")
	file(REMOVE "${OUTPUT_MSIX}")
	if(NOT EXISTS "${OUTPUT_MSIX}")
		message(STATUS "Removed existing MSIX: ${OUTPUT_MSIX}")
	endif()
endif()

execute_process(COMMAND "${MAKEAPPX}" pack /d "${DIST_DIR}" /p "${OUTPUT_MSIX}"
	RESULT_VARIABLE MAKEAPPX_RESULT
	OUTPUT_VARIABLE MAKEAPPX_OUT
	ERROR_VARIABLE MAKEAPPX_ERR
)
if(NOT MAKEAPPX_RESULT EQUAL 0)
	message(FATAL_ERROR "makeappx failed with exit code ${MAKEAPPX_RESULT}\t${MAKEAPPX_OUT}\t${MAKEAPPX_ERR}")
endif()
message(STATUS "Created ${DIST_DIR}/VirtualGamePadPC.msix")

# Helper function to sign MSIX and export certificate
function(sign_and_export_cert PFX_PATH PFX_PASSWORD)
	# Find signtool.exe
	set(_signtool_candidates)
	if(PF86)
		file(GLOB_RECURSE _tmp_signtool "${PF86}/*/signtool.exe" ERROR_QUIET)
		if(_tmp_signtool)
			list(APPEND _signtool_candidates ${_tmp_signtool})
		endif()
	endif()
	if(PF)
		file(GLOB_RECURSE _tmp_signtool2 "${PF}/*/signtool.exe" ERROR_QUIET)
		if(_tmp_signtool2)
			list(APPEND _signtool_candidates ${_tmp_signtool2})
		endif()
	endif()
	if(_signtool_candidates)
		list(SORT _signtool_candidates)
		list(GET _signtool_candidates 0 SIGNTOOL)
	endif()
	if(NOT SIGNTOOL)
		message(FATAL_ERROR "signtool.exe not found on this machine. Cannot sign the MSIX.")
	endif()
	message(STATUS "Using SignTool: ${SIGNTOOL}")

	# Sign MSIX
	execute_process(COMMAND "${SIGNTOOL}" sign /fd SHA256 /a /f "${PFX_PATH}" /p "${PFX_PASSWORD}" /tr http://timestamp.digicert.com /td SHA256 "${DIST_DIR}/VirtualGamePadPC.msix"
		RESULT_VARIABLE SIGN_RES
		OUTPUT_VARIABLE SIGN_OUT
		ERROR_VARIABLE SIGN_ERR
	)
	if(NOT SIGN_RES EQUAL 0)
		message(FATAL_ERROR "signtool failed with exit code ${SIGN_RES}\n${SIGN_OUT}\n${SIGN_ERR}")
	endif()
	message(STATUS "Signed MSIX successfully.")

	# Export public cert and fingerprint
	message(STATUS "Exporting certificate from: ${PFX_PATH}")
	message(STATUS "Output directory: ${DIST_DIR}")
	
	execute_process(COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "$cert = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2('${PFX_PATH}', '${PFX_PASSWORD}'); [System.IO.File]::WriteAllBytes('${DIST_DIR}\\kitswas.cer', $cert.Export([System.Security.Cryptography.X509Certificates.X509ContentType]::Cert)); $certData = [System.IO.File]::ReadAllBytes('${DIST_DIR}\\kitswas.cer'); $sha256 = [System.Security.Cryptography.SHA256]::Create(); $hash = $sha256.ComputeHash($certData); $hashStr = [System.BitConverter]::ToString($hash).Replace('-', ''); [System.IO.File]::WriteAllText('${DIST_DIR}\\kitswas.cer.sha256', $hashStr); $certInfo = $cert.Subject + \"`r`nNotBefore: \" + $cert.NotBefore + \"`r`nNotAfter: \" + $cert.NotAfter; [System.IO.File]::WriteAllText('${DIST_DIR}\\kitswas.cer.info.txt', $certInfo)"
		RESULT_VARIABLE PS_EXPORT_RES
		OUTPUT_VARIABLE PS_EXPORT_OUT
		ERROR_VARIABLE PS_EXPORT_ERR
	)
	message(STATUS "PowerShell export result: ${PS_EXPORT_RES}")
	message(STATUS "PowerShell export output: ${PS_EXPORT_OUT}")
	if(PS_EXPORT_ERR)
		message(STATUS "PowerShell export error: ${PS_EXPORT_ERR}")
	endif()
	
	if(NOT PS_EXPORT_RES EQUAL 0)
		message(WARNING "Failed to export public certificate or fingerprint: ${PS_EXPORT_OUT}\n${PS_EXPORT_ERR}")
	else()
		message(STATUS "Exported public certificate and SHA256 fingerprint.")
	endif()
endfunction()

# Signing: either MSIX_PFX_FILE passed as -D or MSIX_PFX_BASE64 env var method
if(DEFINED MSIX_PFX_FILE AND DEFINED MSIX_PFX_PASSWORD)
		# Normalize the path for PowerShell (forward slashes)
		string(REPLACE "\\" "/" NORMALIZED_MSIX_PFX_FILE "${MSIX_PFX_FILE}")

		# Strip quotes from the path and convert to backslashes for CMake and signtool
		string(REGEX REPLACE "^\"(.*)\"$" "\\1" STRIPPED_MSIX_PFX_FILE "${NORMALIZED_MSIX_PFX_FILE}")
		string(REPLACE "/" "\\" WINDOWS_MSIX_PFX_FILE "${STRIPPED_MSIX_PFX_FILE}")

		# Strip quotes from the password if present
		string(REGEX REPLACE "^\"(.*)\"$" "\\1" STRIPPED_MSIX_PFX_PASSWORD "${MSIX_PFX_PASSWORD}")

	if(EXISTS "${WINDOWS_MSIX_PFX_FILE}")
		message(STATUS "MSIX_PFX_FILE provided: using ${WINDOWS_MSIX_PFX_FILE} to sign the package")
		sign_and_export_cert("${WINDOWS_MSIX_PFX_FILE}" "${STRIPPED_MSIX_PFX_PASSWORD}")
	else()
		message(WARNING "MSIX_PFX_FILE specified but file does not exist: ${WINDOWS_MSIX_PFX_FILE}. Skipping signing.")
	endif()
elseif(DEFINED ENV{MSIX_PFX_BASE64} AND DEFINED ENV{MSIX_PFX_PASSWORD})
	message(STATUS "MSIX_PFX_BASE64 and MSIX_PFX_PASSWORD present: decoding and signing MSIX")
	
	make_directory("temp")
	set(TEMP_PFX_PATH "temp/kitswas.pfx")

	# Decode PFX using PowerShell (write binary)
	execute_process(COMMAND powershell -NoProfile -Command "[System.IO.File]::WriteAllBytes('${TEMP_PFX_PATH}',[System.Convert]::FromBase64String($Env:MSIX_PFX_BASE64))"
		RESULT_VARIABLE PS_DECODE_RES
		OUTPUT_VARIABLE PS_DECODE_OUT
		ERROR_VARIABLE PS_DECODE_ERR
	)

	# Verbose logging for decoding
	message(STATUS "PowerShell decode result: ${PS_DECODE_RES}")
	message(STATUS "PowerShell decode output: ${PS_DECODE_OUT}")
	if(PS_DECODE_ERR)
		message(STATUS "PowerShell decode error: ${PS_DECODE_ERR}")
	endif()

	# Verify the decoded PFX file
	execute_process(COMMAND powershell -NoProfile -Command "Write-Host \"Decoded PFX file size:\"; Get-Item '${TEMP_PFX_PATH}' | Select-Object Length"
		RESULT_VARIABLE PS_FILESIZE_RES
		OUTPUT_VARIABLE PS_FILESIZE_OUT
		ERROR_VARIABLE PS_FILESIZE_ERR
	)
	message(STATUS "PFX file size check result: ${PS_FILESIZE_RES}")
	message(STATUS "PFX file size output: ${PS_FILESIZE_OUT}")
	if(PS_FILESIZE_ERR)
		message(STATUS "PFX file size error: ${PS_FILESIZE_ERR}")
	endif()

	# Attempt to load the certificate
	execute_process(COMMAND powershell -NoProfile -Command "try { $cert = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2('${TEMP_PFX_PATH}', '$Env:MSIX_PFX_PASSWORD'); Write-Host \"Certificate Subject: $($cert.Subject)\" } catch { Write-Host \"Failed to load certificate: $_\" }"
		RESULT_VARIABLE PS_CERTLOAD_RES
		OUTPUT_VARIABLE PS_CERTLOAD_OUT
		ERROR_VARIABLE PS_CERTLOAD_ERR
	)
	message(STATUS "Certificate load check result: ${PS_CERTLOAD_RES}")
	message(STATUS "Certificate load output: ${PS_CERTLOAD_OUT}")
	if(PS_CERTLOAD_ERR)
		message(STATUS "Certificate load error: ${PS_CERTLOAD_ERR}")
	endif()

	sign_and_export_cert("${TEMP_PFX_PATH}" "$ENV{MSIX_PFX_PASSWORD}")

	remove_directory("temp")
else()
	message(WARNING "MSIX signing skipped: MSIX_PFX_BASE64/ENV or MSIX_PFX_FILE not provided.")
endif()

message(STATUS "MSIX creation script completed.")
