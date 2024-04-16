
EXECUTE_PROCESS(
	COMMAND /home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/utils/rapper -q -i rss-tag-soup -o turtle -f writeBaseURI=0 -O http://www.example.org/blog/ file:/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/tests/feeds/test04.atom
	TIMEOUT 10
	OUTPUT_FILE test04.ttl.raw
	ERROR_QUIET
)

# http://public.kitware.com/Bug/view.php?id=13007

CONFIGURE_FILE(
	/home/projects2/vgan_ancient/THIRD_VERSION/SAFARI/deps/raptor/build/tests/feeds/test04.ttl.raw
	test04.ttl
	@ONLY
	NEWLINE_STYLE UNIX
)
