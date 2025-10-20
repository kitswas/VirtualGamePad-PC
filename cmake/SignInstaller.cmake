# SignInstaller.cmake - Signs Windows installer and exports certificate

function(sign_installer INSTALLER_PATH PFX_PATH PFX_PASSWORD OUT_DIR)
	# Strip enclosing quotes from paths and password
	string(REGEX REPLACE "^\"(.*)\"$" "\\1" INSTALLER_PATH "${INSTALLER_PATH}")
	string(REGEX REPLACE "^\"(.*)\"$" "\\1" PFX_PATH "${PFX_PATH}")
	string(REGEX REPLACE "^\"(.*)\"$" "\\1" PFX_PASSWORD "${PFX_PASSWORD}")
	string(REGEX REPLACE "^\"(.*)\"$" "\\1" OUT_DIR "${OUT_DIR}")
	
	# message(STATUS "Signer invoked with ${INSTALLER_PATH}, ${PFX_PATH}, ${PFX_PASSWORD}, ${OUT_DIR}")
	# Find signtool.exe
	set(_signtool_candidates)
	execute_process(COMMAND cmd /C "echo %ProgramFiles(x86)%" OUTPUT_VARIABLE PF86 OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND cmd /C "echo %ProgramFiles%" OUTPUT_VARIABLE PF OUTPUT_STRIP_TRAILING_WHITESPACE)
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
		message(FATAL_ERROR "signtool.exe not found. Cannot sign the installer.")
	endif()
	message(STATUS "Using SignTool: ${SIGNTOOL}")

	# Sign installer
	execute_process(COMMAND "${SIGNTOOL}" sign /fd SHA256 /a /f "${PFX_PATH}" /p "${PFX_PASSWORD}" /tr http://timestamp.digicert.com /td SHA256 "${INSTALLER_PATH}"
		RESULT_VARIABLE SIGN_RES
		OUTPUT_VARIABLE SIGN_OUT
		ERROR_VARIABLE SIGN_ERR
	)
	if(NOT SIGN_RES EQUAL 0)
		message(FATAL_ERROR "signtool failed: ${SIGN_OUT}\n${SIGN_ERR}")
	endif()
	message(STATUS "Signed installer successfully.")

	# Export public cert and fingerprint
	set(CERT_PATH "${OUT_DIR}/installer.cer")
	set(CERT_SHA256_PATH "${OUT_DIR}/installer.cer.sha256")
	set(CERT_INFO_PATH "${OUT_DIR}/installer.cer.info.txt")
	execute_process(
		COMMAND powershell
		-NoProfile
		-ExecutionPolicy Bypass
		-Command
		"$cert = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2('${PFX_PATH}', '${PFX_PASSWORD}'); [System.IO.File]::WriteAllBytes('${CERT_PATH}', $cert.Export([System.Security.Cryptography.X509Certificates.X509ContentType]::Cert)); $certData = [System.IO.File]::ReadAllBytes('${CERT_PATH}'); $sha256 = [System.Security.Cryptography.SHA256]::Create(); $hash = $sha256.ComputeHash($certData); $hashStr = [System.BitConverter]::ToString($hash).Replace('-', ''); [System.IO.File]::WriteAllText('${CERT_SHA256_PATH}', $hashStr); $certInfo = $cert.Subject + \"`r`nNotBefore: \" + $cert.NotBefore + \"`r`nNotAfter: \" + $cert.NotAfter; [System.IO.File]::WriteAllText('${CERT_INFO_PATH}', $certInfo)"
		RESULT_VARIABLE PS_EXPORT_RES
		OUTPUT_VARIABLE PS_EXPORT_OUT
		ERROR_VARIABLE PS_EXPORT_ERR
	)
	if(NOT PS_EXPORT_RES EQUAL 0)
		message(WARNING "Failed to export certificate or fingerprint: ${PS_EXPORT_OUT}\n${PS_EXPORT_ERR}")
	else()
		message(STATUS "Exported public certificate and SHA256 fingerprint successfully to ${CERT_PATH} and ${CERT_SHA256_PATH}.")
	endif()
endfunction()


# Only define options and targets if not in script mode
if(NOT CMAKE_SCRIPT_MODE_FILE)
	option(FILE_TO_SIGN "Path to the installer (.exe/.msi/.msix) to sign." "")
	option(PFX_FILE "Path to a PFX file to sign the installer (optional)." "")
	option(PFX_PASSWORD "Password for the PFX file (optional)." "")
	option(USE_ENV "Use PFX from environment variable instead of PFX_FILE and PFX_PASSWORD." OFF)
	option(SIGN_OUTPUT_DIR "Output directory for signed installer and exported certificate." "${CMAKE_BINARY_DIR}")

	add_custom_target(sign-installer
		COMMAND ${CMAKE_COMMAND} -D FILE_TO_SIGN="${FILE_TO_SIGN}" -D PFX_FILE="${PFX_FILE}" -D PFX_PASSWORD="${PFX_PASSWORD}" -D SIGN_OUTPUT_DIR="${SIGN_OUTPUT_DIR}" -P "${CMAKE_CURRENT_LIST_FILE}" --sign_installer
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Sign Windows installer using PFX file and export certificate."
		VERBATIM
		BYPRODUCTS ${SIGN_OUTPUT_DIR}/installer.cer ${SIGN_OUTPUT_DIR}/installer.cer.sha256 ${SIGN_OUTPUT_DIR}/installer.cer.info.txt
	)

	if(DEFINED ENV{PFX_BASE64} AND DEFINED ENV{PFX_PASSWORD})
		add_custom_target(sign-installer-env
			COMMAND powershell -NoProfile -Command "[System.IO.File]::WriteAllBytes('${SIGN_OUTPUT_DIR}/env_installer.pfx',[System.Convert]::FromBase64String($Env:PFX_BASE64))"
			COMMAND ${CMAKE_COMMAND} -D USE_ENV=ON -D FILE_TO_SIGN="${FILE_TO_SIGN}" -D PFX_FILE="${SIGN_OUTPUT_DIR}/env_installer.pfx" -D PFX_PASSWORD="${PFX_PASSWORD}" -D SIGN_OUTPUT_DIR="${SIGN_OUTPUT_DIR}" -P "${CMAKE_CURRENT_LIST_FILE}" --sign_installer_env
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
			COMMENT "Sign Windows installer using PFX from environment variable and export certificate."
			VERBATIM
			BYPRODUCTS ${SIGN_OUTPUT_DIR}/installer.cer ${SIGN_OUTPUT_DIR}/installer.cer.sha256 ${SIGN_OUTPUT_DIR}/installer.cer.info.txt
		)
	endif()
endif()

if (CMAKE_SCRIPT_MODE_FILE)
	# Script mode execution
	if(NOT USE_ENV)
		if(NOT FILE_TO_SIGN)
			message(FATAL_ERROR "FILE_TO_SIGN is not defined. Cannot sign installer.")
		endif()
		if(NOT PFX_FILE)
			message(FATAL_ERROR "PFX_FILE is not defined. Cannot sign installer.")
		endif()
		if(NOT PFX_PASSWORD)
			message(FATAL_ERROR "PFX_PASSWORD is not defined. Cannot sign installer.")
		endif()
		sign_installer("${FILE_TO_SIGN}" "${PFX_FILE}" "${PFX_PASSWORD}" "${SIGN_OUTPUT_DIR}")
	else()
		if(NOT DEFINED ENV{PFX_BASE64})
			message(FATAL_ERROR "Environment variable PFX_BASE64 is not defined. Cannot sign installer.")
		endif()
		if(NOT DEFINED ENV{PFX_PASSWORD})
			message(FATAL_ERROR "Environment variable PFX_PASSWORD is not defined. Cannot sign installer.")
		endif()
		set(TEMP_PFX_PATH "${SIGN_OUTPUT_DIR}/env_installer.pfx")
		if(NOT FILE_EXISTS "${TEMP_PFX_PATH}")
			message(FATAL_ERROR "Temporary PFX file '${TEMP_PFX_PATH}' does not exist. Cannot sign installer.")
		endif()
		if(NOT FILE_TO_SIGN)
			message(FATAL_ERROR "FILE_TO_SIGN is not defined. Cannot sign installer.")
		endif()
		sign_installer("${FILE_TO_SIGN}" "${TEMP_PFX_PATH}" "$ENV{PFX_PASSWORD}" "${SIGN_OUTPUT_DIR}")
	endif()
endif()
