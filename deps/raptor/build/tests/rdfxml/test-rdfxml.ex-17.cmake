
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/ex-17.rdf http://librdf.org/raptor/tests/ex-17.rdf
	TIMEOUT 10
	OUTPUT_FILE ex-17.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/ex-17.res.raw
	ex-17.res
	@ONLY
	NEWLINE_STYLE UNIX
)
