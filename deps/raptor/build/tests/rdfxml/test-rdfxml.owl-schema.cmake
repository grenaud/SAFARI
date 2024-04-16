
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/owl-schema.rdf http://www.w3.org/2002/07/owl
	TIMEOUT 10
	OUTPUT_FILE owl-schema.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/owl-schema.res.raw
	owl-schema.res
	@ONLY
	NEWLINE_STYLE UNIX
)
