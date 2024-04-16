
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i turtle -o atom -f writeBaseURI=0 -f rssTriples=atom-triples -I http://example.org/news/ -O http://example.org/news/ file:test02.ttl
	TIMEOUT 10
	OUTPUT_FILE test02.atom.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/feeds/test02.atom.raw
	test02.atom
	@ONLY
	NEWLINE_STYLE UNIX
)
