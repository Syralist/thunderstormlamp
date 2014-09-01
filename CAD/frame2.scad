module edge()
{
cube([10,10,10]);
}

module bar(length)
{
color([0,0,length/255],1.0)
cube([length,10,10]);
}

edge();
translate([10,0,0])
bar(200);
translate([210,0,0])
edge();

translate([10,10,0])
rotate([0,0,90])
bar(200);
translate([220,10,0])
rotate([0,0,90])
bar(200);

translate([0,210,0])
edge();
translate([10,210,0])
bar(200);
translate([210,210,0])
edge();

translate([10,220,0])
rotate([0,0,90])
bar(200);
translate([220,220,0])
rotate([0,0,90])
bar(200);

translate([0,420,0])
edge();
translate([10,420,0])
bar(200);
translate([210,420,0])
edge();

translate([10,430,0])
rotate([0,0,90])
bar(200);
translate([220,430,0])
rotate([0,0,90])
bar(200);

translate([0,630,0])
edge();
translate([10,630,0])
bar(200);
translate([210,630,0])
edge();

for (y = [0, 210, 420, 630])
{
translate([120,y,10])
rotate([0,-90,0])
bar(150);
translate([135,y,10])
rotate([0,-45,0])
bar(150);
translate([103,y,17])
rotate([0,-135,0])
bar(150);
translate([-40,y,0])
bar(40);
translate([220,y,0])
bar(40);
translate([110,y,0])
rotate([0,90,0])
bar(150);
translate([127,y,-10])
rotate([0,45,0])
bar(150);
translate([95,y,-2])
rotate([0,135,0])
bar(150);
}





