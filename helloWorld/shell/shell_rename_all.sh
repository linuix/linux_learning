# !/bin/sh
cd ../file
for fileItem in *
do
echo $fileItem
mv "$fileItem" "$fileItem".pcm
done