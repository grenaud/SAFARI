
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples file:/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/warn-02.rdf http://librdf.org/raptor/tests/warn-02.rdf
	TIMEOUT 10
	OUTPUT_FILE warn-02.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/warn-02.res.raw
	warn-02.res
	@ONLY
	NEWLINE_STYLE UNIX
)
