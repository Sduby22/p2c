workdir=`pwd`
parser_root="../"
cd $parser_root
echo 'building p2c module'
bash buildall.sh
mkdir -p $workdir'/packages/renderer/src/wasm/'
cp bin/libparser.* $workdir'/packages/renderer/src/wasm/'
echo 'output copied.'
