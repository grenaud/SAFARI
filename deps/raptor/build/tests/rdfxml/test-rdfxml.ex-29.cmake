
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/ex-29.rdf http://librdf.org/raptor/tests/ex-29.rdf
	TIMEOUT 10
	OUTPUT_FILE ex-29.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/ex-29.res.raw
	ex-29.res
	@ONLY
	NEWLINE_STYLE UNIX
)
