
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -o ntriples /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/rdfxml/rdfs-namespace.rdf http://www.w3.org/2000/01/rdf-schema
	TIMEOUT 10
	OUTPUT_FILE rdfs-namespace.res.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/rdfxml/rdfs-namespace.res.raw
	rdfs-namespace.res
	@ONLY
	NEWLINE_STYLE UNIX
)
