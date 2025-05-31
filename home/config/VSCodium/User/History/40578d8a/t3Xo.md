# Results

## V0-1
Iterations: 5 - Max: 49.196565 - Min: 48.947439 - Avg: 49.0420658

## V0-2

Matrices: Random: C: 1000x500 Q: 1000x500 K:32
Block Size: 5 - I: 50 - Max: 0.6680150000 - Min: 0.5989640000 - Avg: 0.6295022600

## V0-3

Random C:1000x500 Q:1000x500 K:32 Iterations:20
Block Size: 8     - Max: 0.5095190000 - Min: 0.4879950000 - Avg: 0.5001907000
Block Size: 16    - Max: 0.5053660000 - Min: 0.4808130000 - Avg: 0.4948623500
Block Size: 32    - Max: 0.4979490000 - Min: 0.4729290000 - Avg: 0.4831860500
Block Size: 64    - Max: 0.6799570000 - Min: 0.5352510000 - Avg: 0.5806488500
Block Size: 128   - Max: 0.5600220000 - Min: 0.5041850000 - Avg: 0.5383432000
Block Size: 256   - Max: 0.5743580000 - Min: 0.4726900000 - Avg: 0.5006549500
Block Size: 512   - Max: 0.4872890000 - Min: 0.4468520000 - Avg: 0.4650303500
Block Size: 1024  - Max: 0.4786210000 - Min: 0.4460040000 - Avg: 0.4599408000
Block Size: 2048  - Max: 0.4786810000 - Min: 0.4468750000 - Avg: 0.4628779500
Block Size: 4096  - Max: 0.4794550000 - Min: 0.4513720000 - Avg: 0.4624255000
Block Size: 8192  - Max: 0.4827140000 - Min: 0.4451650000 - Avg: 0.4664678000
Block Size: 16384 - Max: 0.4735430000 - Min: 0.4440330000 - Avg: 0.4582451500

Block Size: 30 - Max: 0.5082150000 - Min: 0.4749780000 - Avg: 0.4944928500
Block Size: 40 - Max: 0.6814520000 - Min: 0.5483330000 - Avg: 0.6345676000
Block Size: 50 - Max: 0.6590930000 - Min: 0.5807730000 - Avg: 0.6156314000

Random C:10000x500 Q:10000x500 K:64 Iterations:5
Block Size: 32    - Max: 51.4285300000 - Min: 48.9412320000 - Avg: 50.5068016000
Block Size: 64    - Max: 62.4787820000 - Min: 57.5272400000 - Avg: 59.8954686000
Block Size: 1024  - Max: 44.5681030000 - Min: 43.9710660000 - Avg: 44.2645232000
Block Size: 16384 - Max: 49.5126630000 - Min: 46.7186600000 - Avg: 47.9319158000

Apparently spliting to multiple source files makes it on average .1% faster.


before sqr out of loop:
iter:5 - block:32 - Max: 32.4224520000 - Min: 30.0507770000 - Avg: 31.3675974000
After:
iter:5 - block:32 - Max: 33.2282110000 - Min: 30.0891820000 - Avg: 31.6113864000
After flattening neighbors:
iter:5 - block:32 - Max: 33.0866970000 - Min: 30.5725710000 - Avg: 32.2350020000
After precalculating norms in function:
iter:5 - block:32 - Max: 4.9361450000 - Min: 4.6086710000 - Avg: 4.8136406000   800% boost!
After precalulating norms per index:
iter:5 - block:32 - Max: 4.0013070000 - Min: 3.8108160000 - Avg: 3.9271592000