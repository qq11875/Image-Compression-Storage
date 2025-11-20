make

fileName=(dodo)
mode=ebcBlock
remove=1

rm tmp.ebcc tmp.ebu tmp.ebf tmp.ebc

export MallocStackLogging reset

echo "./ebcBlock sample_ebc_images/$fileName.ebc tmp.ebcc"
./ebcBlock sample_ebc_images/$fileName.ebc tmp.ebcc

echo "./ebcUnblock tmp.ebcc tmp.ebc"
./ebcUnblock tmp.ebcc tmp.ebc

echo "./ebc2ebu tmp.ebc tmp.ebu"
./ebc2ebu tmp.ebc tmp.ebu

echo "./ebu2ebf tmp.ebu tmp.ebf"
./ebu2ebf tmp.ebu tmp.ebf


#rm tmp.ebcc tmp.ebu tmp.ebf


make clean