<?php
$x=(int)$_GET["x"];
$y=(int)$_GET["y"];
$z=(int)$_GET["z"];
$im=imagecreatefrompng("http://ingreelab.net/C04AF0B62BEC112E8D7242FB848631D12D252728/$z/$x/$y.png");
/*imagefilter($im, IMG_FILTER_GRAYSCALE);
imagefilter($im, IMG_FILTER_CONTRAST, 40);
imagefilter($im, IMG_FILTER_BRIGHTNESS, 80);
imagefilter($im, IMG_FILTER_GRAYSCALE);*/

/*$width=imagesx($im);
$height=imagesy($im);
for($y=0; $y<$height; $y++)
    for($x=0;$x<$width;$x++) {
        $color=imagecolorat($im, $x, $y);
        var_dump($color);
        $r=$color>>16;
        $g=($color>>8)&255;
        $b=$color&255;
        imagesetpixel($im, $x, $y, $color);
    }*/
    
for($i=0;$i<imagecolorstotal($im); $i++) {
    $color=imagecolorsforindex($im, $i);
    $r=$color["red"];
    $g=$color["green"];
    $b=$color["blue"];
    $gr=(int)(255*2/3+($r+$g+$b)/3/3);
    imagecolorset($im, $i, $gr, $gr, $gr);
}
    
header("Content-type: image/png");
imagepng($im);
?>