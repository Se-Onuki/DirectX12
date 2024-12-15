# pngファイルとjpgファイルを取得
$files = Get-Item *.png, *.jpg
# 取得したファイルをddsに変換するexeに渡す
foreach($f in $files){
    Start-Process -FilePath Exe/TextureConverter.exe -ArgumentList $f -Wait
}
pause