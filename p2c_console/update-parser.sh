workdir=`pwd`
parser_root="../p2c"
cd $parser_root
echo 'building p2c module'
bash buildall.sh
cp bin/libparser.* $workdir'/packages/renderer/src/wasm/'
echo 'output copied.'
