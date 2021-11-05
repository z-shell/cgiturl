set( IMANIFEST "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt" )

if( NOT EXISTS ${IMANIFEST} )
    message( "! Cannot find install manifest, project wasn't installed?" )
    message( FATAL_ERROR "Install manifest should be at: ${IMANIFEST}" )
endif()

file( STRINGS ${IMANIFEST} ifiles )
foreach( ifile ${ifiles} )
    if( EXISTS ${ifile} )
        message( "= Uninstalling file: ${ifile}" )

        exec_program( ${CMAKE_COMMAND} ARGS "-E remove ${ifile}" OUTPUT_VARIABLE stdout RETURN_VALUE result )

        if( NOT "${result}" STREQUAL 0 )
            message( FATAL_ERROR "Failed to remove file: ${ifile}" )
        endif()
    else()
        message( "= ${ifile} does not exist" )
    endif()
endforeach(ifile)
