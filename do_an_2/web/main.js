const firebaseConfig = {
    apiKey: "AIzaSyBwq12cM8LljvLcYjWKca9XGXnbtP59ycU",
    authDomain: "doan2-af66d.firebaseapp.com",
    databaseURL: "https://doan2-af66d-default-rtdb.firebaseio.com",
    projectId: "doan2-af66d",
    storageBucket: "doan2-af66d.appspot.com",
    messagingSenderId: "646716658494",
    appId: "1:646716658494:web:5276b421181f59b505d26f",
    measurementId: "G-D91H9Z6E0P"
  };

  firebase.initializeApp(firebaseConfig);
  var database= firebase.database();

  database.ref("/plsp/weight").on("value", function(snapshot){
    var cn=snapshot.val();
    document.getElementById("weight").innerHTML=cn + "   G";
  });

  database.ref("/plsp/soluongsp1").on("value", function(snapshot){
    var slsp1=snapshot.val();
    document.getElementById("demsp1").innerHTML=slsp1 + "   SP";
  });

  database.ref("/plsp/soluongsp2").on("value", function(snapshot){
    var slsp2=snapshot.val();
    document.getElementById("demsp2").innerHTML=slsp2 + "   SP";
  });

  database.ref("/plsp/soluongsp3").on("value", function(snapshot){
    var slsp3=snapshot.val();
    document.getElementById("demsp3").innerHTML=slsp3 + "   SP";
  });


