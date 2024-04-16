
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i turtle -o atom -f writeBaseURI=0 -f rssTriples=atom-triples -f atomEntryUri=http://example.org/2006/03/28/blog-item -I http://example.org/whatsnew -O http://example.org/whatsnew file:test01.ttl
	TIMEOUT 10
	OUTPUT_FILE test01.atom.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/feeds/test01.atom.raw
	test01.atom
	@ONLY
	NEWLINE_STYLE UNIX
)
