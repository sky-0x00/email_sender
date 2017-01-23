$wc = new-object System.Net.WebClient
$url="http://myexternalip.com/raw"
$wc.DownloadString($url)