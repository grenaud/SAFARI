Some of us would like to have an VG of the Ensembl E.coli bacterial genomes that so that we can see what mapped annotations from Ensembl&UniProt would look like on a VG datamodel.

# Getting started: making a VG from a bunch of E.coli genomes.

```bash
mkdir ecoli
cd ecoli
#k12
ensemblbac33fasta="ftp://ftp.ensemblgenomes.org/pub/bacteria/release-33/fasta/bacteria_9_collection"
wget "$ensemblbac33fasta/escherichia_coli_str_k_12_substr_dh10b/dna/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.dna.chromosome.Chromosome.fa.gz"
#o157
wget "$ensemblbac33fasta/escherichia_coli_o157_h7_str_sakai/dna/Escherichia_coli_o157_h7_str_sakai.ASM886v1.dna.chromosome.Chromosome.fa.gz"

#some downstream tools do not like gzipped files
gunzip Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.dna.chromosome.Chromosome.fa.gz
gunzip Escherichia_coli_o157_h7_str_sakai.ASM886v1.dna.chromosome.Chromosome.fa.gz
```

As the Ensebml fasta is a bit odd we first need to change the header to have the assembly id be the first part of the header.
e.g.
```
>ASM1942v1 C1:Chromosome dna:chromosome chromosome:ASM1942v1:Chromosome:1:4686137:1 REF
```


```bash
vg construct \
  --reference ecoli/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.dna.chromosome.Chromosome.fa \
  -m 1000 \
> ecoli/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.vg
```
We are making a reference graph from the first E.coli genome and limit the node size to 1000.

Then its time to index the E.coli VG which we started with as root path in the vg graph.
```bash 
vg index -x ecoli/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.xg \
  -g ecoli/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.gcsa \
  -k 11 \
  ecoli/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.vg
```


Then its time to align other ecoli genomes to the first one

```bash
vg msga \
  -g ecoli/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.vg \
  -f ecoli/Escherichia_coli_o157_h7_str_sakai.ASM886v1.dna.chromosome.Chromosome.fa \
> ecoli/k12_and_o157.vg
```

This gives you a vg file with two paths. Quick sanity check is to validate that we have nodes of varying length. e.g.
```bash
for i in $(vg view -t ecoli/k12_and_o157.vg | grep rdf:value | cut -f 2 -d '"');
do 
  echo ${#i};
done|sort -nu
```
## Getting Ensembl genes into the graph

Get BED file from [Ensembl Genomes (click export button)](http://bacteria.ensembl.org/Escherichia_coli_str_k_12_substr_dh10b/Location/Overview?r=Chromosome:1283257-1283991)

Then the BED file first column needs to correspond with your VG paths, you can check the paths in your vg file with this command

```bash
vg paths -L  -v k12_and_o157.vg 
```

The bed file, from Ensembl genomes, first column will have the value `Chromosome` this needs to 
be changed to match the name of the path in the VG graph representing that Chromosome. I use this quick sed/bash one liner.

```
sed 
  "s/Chromosome/Escherichia_coli_str_k_12_substr_dh10b.ASM1942v:Chromosome/" \
    Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.bed > 
    Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.bed.fixed
```

We then use the VG annotate command to make a new file in VG speak a "GAM file".

```bash
vg annotate -p \
　　　　　　　　　　　　　　　　　　　　　　　　-x k12_and_o157.xg　\
　　　　　　　　　　　　　　　　　　　　　　 -b Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.bed.fixed >
　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.gam
```
This is merged back into a new vg file using the command `vg mod`

```
vg mod -i Escherichia_coli_str_k_12_substr_dh10b.ASM1942v1.gam k12_and_o157.vg > k12_and_o157_v2.vg
```

We then need to make a new XG index to this v2 VG file.

```
vg index -x k12_and_o157_v2.xg k12_and_o157_v2.vg 
```

We then fix the second bed file from ensembl genomes to make the 1st column match the path name in our VG file

```
sed "s/Chromosome/Escherichia_coli_o157_h7_str_sakai.ASM886v1:Chromosome/" Escherichia_coli_o157_h7_str_sakai.ASM886v1.bed > Escherichia_coli_o157_h7_str_sakai.ASM886v1.bed.fixed   
```

We then run annotate again.

```
vg annotate -p \
            -x k12_and_o157_v2.xg \
            -b Escherichia_coli_o157_h7_str_sakai.ASM886v1.bed.fixed >
               Escherichia_coli_o157_h7_str_sakai.ASM886v1.gam
```
And vg mod
```
vg mod -i Escherichia_coli_o157_h7_str_sakai.ASM886v1.gam k12_and_o157_v2.vg　> k12_and_o157_v3.vg

```

We then can get RDF out of the VG file v3 (step3) with vg view -t k12_and_o157_v3.vg.





# Preparing Ensembl and VG RDF

We get the Ensembl RDF from
```bash
ensembl33bac9="ftp://ftp.ensemblgenomes.org/pub/release-33/bacteria/rdf/bacteria_9_collection"
wget "$ensembl33bac9/escherichia_coli_str_k_12_substr_dh10b/escherichia_coli_str_k_12_substr_dh10b.ttl.gz"
wget "$ensembl33bac9/escherichia_coli_o157_h7_str_sakai/escherichia_coli_o157_h7_str_sakai.ttl.gz"
```

We turn the merged vg, with ensembl gene paths, into rdf with `vg view`

```
./vg view -t ./ecoli/k12_and_o157_v3.vg > ./ecoli/k12_and_o157_v3.ttl
```
Compress it if you want even if its tiny
```
xz -T 4 ./ecoli/k12_and_o157_v3.ttl
```

Assuming recent virtuoso set up loading assume data is in ${HOME}/ecoli it needs to be an absolute path.

```bash
isql $port $user $pass \ 
    exec="ld_dir('${HOME}/ecoli','k12_and_o157.ttl.xz', 'http://example.org/vg_ecoli')"
isql $port $user $pass \
    exec="ld_dir('${HOME}/ecoli','escherichia_coli_o157_h7_str_sakai.ttl.gz','http://example.org/ensembl_ecoli')"
isql $port $user $pass \
    exec="ld_dir('${HOME}/ecoli','escherichia_coli_str_k_12_substr_dh10b.ttl.gz','http://example.org/ensembl_ecoli')"
isql $port $user $pass \
    exec="rdf_loader_run(log_enable=>2);"
```
# Checking the VG and Ensembl data 

Sanity check the loaded VG data
```sparql
SELECT
  (COUNT(?forward) as ?forward_nodes)
  (COUNT(?reverse) as ?reverse_nodes) 
WHERE {
  {?forward a <http://example.org/vg/Forward>} 
    UNION
  {?reverse a <http://example.org/vg/Reverse>}
}
```

```sparql
SELECT
  DISTINCT ?path
WHERE {
  ?node <http://example.org/vg/path> ?path
}
```

linking up the `faldo:reference` set in Ensembl RDF and the `vg:path`
```sparql
PREFIX vg:<http://example.org/vg/>
PREFIX faldo:<http://biohackathon.org/resource/faldo#>
SELECT 
  DISTINCT ?path  
WHERE {
  ?node vg:path|faldo:reference ?path 
}
```
Gives the chromosomes, plasmids and paths as in the db.

Get the start and end positions of all protein coding genes on the Ecoli sakai chromosome that is a path in our VG graph.

```sparql
PREFIX owl:<http://www.w3.org/2002/07/owl#> 
PREFIX faldo:<http://biohackathon.org/resource/faldo#> 
SELECT 
  ?s ?bp ?ep
WHERE {
 ?s a <http://purl.obolibrary.org/obo/SO_0001217>; 
   faldo:location ?l . 
 ?l faldo:end ?e ; faldo:begin ?b .
 ?e faldo:position ?ep ; faldo:reference ?chromo . 
 ?b faldo:position ?bp ; faldo:reference ?chromo . 
 <http://example.org/vg//path/ASM886v1> owl:sameAs ?chromo 
}
```

# Introducing links between VG and Ensembl RDF.

We just use skos:closeMatch for now.

```sparql
prefix vg:<http://example.org/vg/>
prefix skos: <http://www.w3.org/2004/02/skos/core#>
prefix dc: <http://purl.org/dc/elements/1.1/>
prefix dcterm: <http://purl.org/dc/terms/>
prefix obo: <http://purl.obolibrary.org/obo/> 


INSERT
	{?path skos:closeMatch ?transcript }
WHERE 
  	{
      ?s vg:path ?path . 
      BIND (STRBEFORE(SUBSTR(STR(?path),28),'-') AS ?pathId)
      BIND (IRI(CONCAT('http://rdf.ebi.ac.uk/resource/ensembl.transcript/', ?pathId)) AS ?transcript)
    }
```

# Real work with VG and Ensembl RDF

Find the transcripts/genes in Ensembl that share nodes with an other one.
```sparql
prefix vg:<http://example.org/vg/>
prefix skos: <http://www.w3.org/2004/02/skos/core#>
prefix dc: <http://purl.org/dc/elements/1.1/>
prefix dcterm: <http://purl.org/dc/terms/>
prefix obo: <http://purl.obolibrary.org/obo/> 


SELECT
	DISTINCT ?transcript1 ?transcript2
WHERE
	{
    ?path1 skos:closeMatch ?transcript1 ;
           ^vg:path ?step1 .
    ?step1 vg:node ?node  .
    ?transcript1 a obo:SO_0000234 .    
    ?path2 skos:closeMatch ?transcript2 ;
           ^vg:path ?step2 .
    ?step2 vg:node ?node  .
    ?transcript2 a obo:SO_0000234 .    
    FILTER (! sameTerm(?path1, ?path2))  
}
```


```sparql
prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#>
prefix vg:<http://example.org/vg/>
prefix skos: <http://www.w3.org/2004/02/skos/core#>
prefix dc: <http://purl.org/dc/elements/1.1/>
prefix dcterm: <http://purl.org/dc/terms/>
prefix obo: <http://purl.obolibrary.org/obo/> 
prefix up: <http://purl.uniprot.org/core/>


SELECT
	?transcript ?uniprot ?comment
{
  ?transcript a obo:SO_0000234 .  
  ?uniprot rdfs:seeAlso ?transcript ;
           up:annotation ?annotation .
  ?annotation a up:Function_Annotation ;
                rdfs:comment ?comment .
  
}
```

