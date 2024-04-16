
SET(rdfdiff_status -1)

EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -f noNet -q -i rdfa10 -I http://rdfa.info/test-suite/test-cases/xhtml1/rdfa1.0/0088.xml -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfa/0088.xml
	TIMEOUT 10
	OUTPUT_FILE 0088-res.nt
	ERROR_QUIET
	RESULT_VARIABLE rapper_status
)

IF(rapper_status EQUAL 0 OR rapper_status EQUAL 2)
	EXECUTE_PROCESS(
		COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rdfdiff -f ntriples -u http://rdfa.info/test-suite/test-cases/xhtml1/rdfa1.0/0088.xml -t ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfa/0088.out 0088-res.nt
		TIMEOUT 10
		OUTPUT_QUIET
		ERROR_QUIET
		RESULT_VARIABLE rdfdiff_status
	)
ENDIF(rapper_status EQUAL 0 OR rapper_status EQUAL 2)

IF(0)
  IF(rdfdiff_status EQUAL 0)
    MESSAGE(FATAL_ERROR "Test succeeded, but expected to fail.")
  ENDIF(rdfdiff_status EQUAL 0)
ELSE()
  IF(NOT rdfdiff_status EQUAL 0)
    MESSAGE(FATAL_ERROR "Test failed.")
  ENDIF(NOT rdfdiff_status EQUAL 0)
ENDIF()
