set -e

work_dir=`pwd`
echo $work_dir
pushd $work_dir

tmp_file="${work_dir}/.tmp"

the3rdparty_root="${work_dir}/../../3rdparty"

echo '======= the version of 3rdpartys =======' > ${tmp_file}
libs="libmtcvlite libyuv libManis libmtimage"
for lib in ${libs}
do
    cd ${the3rdparty_root}/${lib}
    echo "\n======= ${lib} =======" >> ${tmp_file}
    git branch >> ${tmp_file}
done

cat ${tmp_file}
rm ${tmp_file}
popd