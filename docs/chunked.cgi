#!/usr/bin/perl
# 上の１行の前には空行も空白文字もはいらないようにしてください。
# perlのパス名はプロバイダや環境に合わせて変更してください。

#
# HTMLのメタ文字をエスケープする関数を用意します
#
sub html {
    $str = $_[0];
    $str =~ s/&/&amp;/g;
    $str =~ s/</&lt;/g;
    $str =~ s/>/&gt;/g;
    $str =~ s/"/&quot;/g;
    $str =~ s/'/&#x27;/g;
    return $str;
}

#
# ヘッダ部を書き出します
#
print "Content-Type: text/html\nTransfer-Encoding: chunked\n\nB\r\nmcgee mine\n\r\n0\r\n\r\n"
