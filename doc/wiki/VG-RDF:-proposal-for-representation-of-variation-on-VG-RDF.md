By describing variants explicitly on VG-RDF, connecting with dbSNP and any other resources can be easy, and we can infer the relationship between nodes.

```
  / ACTG  \
 -         -
  \ AC-GG /
      \  /
       GT
```

* Path1(P1): ACTG
* Path2(P2): ACGG
* Path3(P3): ACGT

In this case, we discussed the representation like the following  (? is a draft).

```
<P3S1>     : rank 1
          ?: difference <P3S1P1S1> 
<P3S1P1S1>?: deletes <P1S1>
          ?: inserts <P3S2>, <P3S1>
<P1S1>     : rank 1
          ?: difference <P1S1P3S1>
<P1S1P3S1>?: deletes <P3S2>, <P3S1>
          ?: inserts <P1S1>
          ?: opposite <P3S1P1S1>
          A ?: MNP?
            ?: LETHAL_MUTATION?
```

We can easily associate with existing DBs like as follows.

```
dbSNP:xyz owl:SAMEAS <P1S1P3S1>
```

It is similar to the concept of edit of VG, but the "difference" representation in RDF only implies the relationship between nodes/paths, not intend to modify graphs.

Moreover, by describing multiple variants as one object on VG RDF,  we can also associate a chain of variants to annotations.

----

This is an alternative idea; it can describe the type of variant explicitly, but it seems more complex than the previous one.

```
Node_id: 101, 102, 103   -- a reference path
         ACG - A - CT
           \      /
               G (104)   -- a patient path
```

```
<P1S1> : rank 1
       : path <P1>
       : node <101>
<P1S2> : rank 2
       : path <P1>
       : node <102>
      ?: variant <P2S2>
<P2S2> : rank 2      
       : path <P2>
       : node <102>
      ?: variant <P1S2>
```

For describing insertion sequence:

```
<P3S2>: rank 2
      : path <P3>
      : node <103>
      : insertedBefore <P1S2>
      : insertedAfter <P1S1>
      : variant <P2S2>
```