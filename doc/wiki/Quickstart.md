**N.B.**: This guide won't work for a large whole genomes (like human), but peruse it before heading to [[working-with-a-whole-genome-variation-graph]].


            ## Check out all the cool things vg can do!
            ./bin/vg

            ## see detailed usage instructions for
            ## any command by typing its name w/ no arguments
            ./bin/vg construct
            
            ## Construct - take a reference genome and a VCF and build a graph
            ## VCF must be bgzip'ed and tabix-indexed
            ./bin/vg construct -r reference.fa -v variants.vcf.gz > graph.vg

            ## Maybe you want to see what it looks like?
            ## Better have graphtools installed though.
            ./bin/vg view -d graph.vg | dot -Tpdf -o graph.pdf

            ## Want to map reads? We need two kinds of index first
            ./bin/vg index -x graph.xg -g graph.gcsa -k 11 graph.vg

            ## Ok, now that we have graph.xg and graph.gcsa we can map some reads.
            ## for single end reads:
            ./bin/vg map -x graph.xg -g graph.gcsa -f se_reads.fq > se_reads.gam

            ## For paired-end reads, put your forward reads first and just
            ## use a second -f for your reverse reads
            ./bin/vg map -x graph.xg -g graph.gcsa -f pe_reads_1.fq -f pe_reads_2.fq > pe_reads.gam

            ## Want to call some variants in those reads? Try vg genotype.
            ## First we need to index our new GAM file.
            ## This will make what looks like a directory,
            ## named <my_gam>.index/
            ./bin/vg index -d pe_reads.gam.index -N pe_reads.gam

            ## Now we can genotype our reads. Use -v to get VCF output
            ./bin/vg genotype -v graph.vg pe_reads.gam.index/ > calls.vcf