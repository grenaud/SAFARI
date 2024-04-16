
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/22-rdf-syntax-ns.rdf http://www.w3.org/1999/02/22-rdf-syntax-ns
	TIMEOUT 10
	OUTPUT_FILE 22-rdf-syntax-ns.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/22-rdf-syntax-ns.res.raw
	22-rdf-syntax-ns.res
	@ONLY
	NEWLINE_STYLE UNIX
)
