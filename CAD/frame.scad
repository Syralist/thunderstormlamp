use <plate.scad>;
use <bar.scad>;

basex = 10;
basey = 5;
basez = 1;

barwidth = 1;
barlength = 6;

barangle = 30;

plate(basex, basey, basez);

translate([-(basex/4),
			-(barlength/2)*sin(barangle)-basey/2,
			(barlength/2)*cos(barangle)])
rotate([barangle,0,0])
bar(barwidth, barlength);
translate([-(basex/4),
			(barlength/2)*sin(barangle)+basey/2,
			(barlength/2)*cos(barangle)])
rotate([-barangle,0,0])
bar(barwidth, barlength);
translate([(basex/4),
			-(barlength/2)*sin(barangle)-basey/2,
			(barlength/2)*cos(barangle)])
rotate([barangle,0,0])
bar(barwidth, barlength);
translate([(basex/4),
			(barlength/2)*sin(barangle)+basey/2,
			(barlength/2)*cos(barangle)])
rotate([-barangle,0,0])
bar(barwidth, barlength);
translate([-(basex/4),
			-(barlength/2)*sin(barangle)-basey/2,
			-(barlength/2)*cos(barangle)])
rotate([-barangle,0,0])
bar(barwidth, barlength);
translate([-(basex/4),
			(barlength/2)*sin(barangle)+basey/2,
			-(barlength/2)*cos(barangle)])
rotate([barangle,0,0])
bar(barwidth, barlength);
translate([(basex/4),
			-(barlength/2)*sin(barangle)-basey/2,
			-(barlength/2)*cos(barangle)])
rotate([-barangle,0,0])
bar(barwidth, barlength);
translate([(basex/4),
			(barlength/2)*sin(barangle)+basey/2,
			-(barlength/2)*cos(barangle)])
rotate([barangle,0,0])
bar(barwidth, barlength);

translate([barlength/2+basex/2,0,0])
rotate([0,90,0])
bar(barwidth, barlength);


translate([-(barlength*1.5/2+basex/2),0,0])
rotate([0,90,0])
scale([1,1,1.5])
bar(barwidth, barlength);


