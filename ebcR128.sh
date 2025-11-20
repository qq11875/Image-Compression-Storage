make

fileName=(dodo)
seed=721831
mode=ebcBlock
remove=1

rm tmp.ebcc tmp.ebu tmp.ebf tmp.ebc

./ebf2ebu sample_ebf_images/$fileName.ebf tmp.ebu
./ebu2ebc tmp.ebu tmp.ebc

echo "./ebcR128 sample_ebc_images/$fileName.ebc tmp.ebcc $seed"
./ebcR128 tmp.ebc tmp.ebcc $seed
rm tmp.ebu

echo "./ebcU128 tmp.ebcc tmp.ebc"
./ebcU128 tmp.ebcc tmp.ebc

echo "./ebc2ebu tmp.ebc tmp.ebu"
./ebc2ebu tmp.ebc tmp.ebu

echo "./ebu2ebf tmp.ebu tmp.ebf"
./ebu2ebf tmp.ebu tmp.ebf


# rm tmp.ebcc tmp.ebu tmp.ebf


make clean