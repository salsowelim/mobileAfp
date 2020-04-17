/*Copyright (c) 2020, Suliman Alsowelim
All rights reserved.
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree. 
*/
import React, { Component } from 'react';
import { StyleSheet, Text, View, TouchableOpacity, NativeModules } from 'react-native';

export default class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      status: 0, //0 not recording, 1, recording, 2, display result
    };
  }
  onPress = () => {
      test_json = `[{
    "hash": "0",
    "offset": 0
    }]`;
    fp = async () => {
      try {
        const result_string = await NativeModules.RecAndFp.fp();
        test_json = result_string;
        console.log(test_json);
      } catch ( e ) {
        console.error(e);
      }
    };

    if (this.state.status == 1) {
      NativeModules.RecAndFp.stop_recording();
      fp();
      this.setState({status: 2});
    } else {
      //start recording 
      NativeModules.RecAndFp.start_recording();
      this.setState({status: 1});   
      }
  };
  render() {
      if (this.state.status == 0){  
      return ( <View style={{flex: 1}}>
    <View style={{flex: 1}}>
    </View>
        <View style={{flex: 2}}>
          <TouchableOpacity
                              onPress={ this.onPress }
                            style={{borderRadius: 100}}>
         
          <View style={{backgroundColor: 'blue',
    borderRadius: 65,
    height: 120,
    width: 120,
    alignSelf: "center"
        }}>
          </View>
          </TouchableOpacity>
        </View>
          <View style={{flex: 2, margin: 20 }} >
          <Text style={{fontSize: 25}} >{"press to start recording, you have to have record permission first."}</Text>
          </View>
        </View>);
    }else if (this.state.status == 1){
      // now recording
       return ( <View style={{flex: 1}}>
 <View style={{flex: 1}}>
    </View>
        <View style={{flex: 2}}>
          <TouchableOpacity
                              onPress={ this.onPress }
                            style={{borderRadius: 100}}>
         
          <View style={{backgroundColor: 'blue',
    borderRadius: 65,
    height: 120,
    width: 120,
    alignSelf: "center"
        }}>
          </View>
          </TouchableOpacity>
        </View>
          <View style={{flex: 2,  margin: 20 }} >
          <Text style={{fontSize: 25}} >{"Recording, after some time press once to stop and wait a sec for fingerprinting, or else app will crash eventually"}</Text>
          </View>
        </View>);
  
    }else {
      //fingerprinting
       return ( <View style={{flex: 1}}>
          <View style={{flex: 1 ,  margin: 20 }} >
          <Text style={{fontSize: 25}}>{"fingerprints logged on console. Check it using \"react-native log-android\". restart the app to try again"}</Text>
          </View>
        </View>);
  }
}
}
