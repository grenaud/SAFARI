
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/daml-oil.rdf http://www.daml.org/2001/03/daml+oil
	TIMEOUT 10
	OUTPUT_FILE daml-oil.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/daml-oil.res.raw
	daml-oil.res
	@ONLY
	NEWLINE_STYLE UNIX
)
