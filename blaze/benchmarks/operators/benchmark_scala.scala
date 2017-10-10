
//object Cells {
  import org.apache.spark.SparkContext
  import org.apache.spark.sql.SQLContext
  import org.apache.spark.SparkConf
  
  //val conf = new SparkConf().setMaster("local[1]")
  
  //val sparkContext = new SparkContext(conf);


  def time[R](block: => Array[org.apache.spark.sql.Row]) {
      val t0 = System.nanoTime();
      var result = block    // call-by-name
      result = Array()
      val t1 = System.nanoTime();
      println("Elapsed time: " + (t1 - t0)/1000000 + "ms");
  }
  val sqlc = spark;


  val MAX = 1 << 26;
  val join1 = 1 << 15;
  val join2 = 1 << 20;



import org.apache.spark.sql.functions.rand;


println("input size " + MAX);
val input_1 = sqlc.range(0, MAX).select(rand.alias("c0")).cache();


var temp = input_1.collect();
temp = Array();
println("initialized input 1");

  
  
  //sum
  
//println("sum");
//time(input_1.agg(sum("c0")).collect());

  /* 

  //map
println("map");
  time(input_1.select(col("c0") * 3 + 7).collect());

  

  //filter
println("filter");
  time(input_1.filter(col("c0") > 0.5).collect());


  //map filter
  
println("mapfilter");
  time(input_1.select(col("c0") * 3 + 7).filter(col("c0") > 0.5).collect());
*/

//filter sum
println("filter.sum");
  time(input_1.filter(col("c0") > 0.5).sum().collect());





/*
  //join
  input_1.unpersist();
  

println("init join");
  val input_join1 = sqlc.range(0, join1).select((rand * 1000).alias("c0").cast("long"), (rand* 1000).alias("c1").cast("long")).cache();
  val input_join2 = sqlc.range(0, join2).select((rand * 1000).alias("c0").cast("long"), (rand* 1000).alias("c1").cast("long")).cache();
  {
  var t = input_join1.collect();
  t = Array();
  var t2 = input_join2.collect();
  t2 = Array();
  }
println("initialized join input");
  
println("join");
  
  time(input_join1.join(input_join2, "c0").collect());



  //map filter join
  
  input_join2.unpersist();
  val input_join2_ = sqlc.range(0, join2).select((rand() * 1000).cast("long").alias("c0")).cache();
  {
  var t = input_join2_.collect();
  t = Array()
  }
  
println("map filter join");
  
  val d2 = input_join2_.select(col("c0") ,col("c0") * 3 + 7).filter(col("c0") > 0.5);
  time(input_join1.join(d2, "c0").collect());

  /* 
  
    
  //reduce by key
  val input_reduce_by_key = sqlc.range(0, MAX >> 1).select((rand() * 10).cast("long").alias("c0"), (rand() * 10).cast("long").alias("c1")).cache();
  
  {
  var res2 = input_reduce_by_key.collect();
res2 = Array();
}

println("reduce by key");
time(input_reduce_by_key.groupBy("c0").agg(sum("c1")).collect());
  
  */





