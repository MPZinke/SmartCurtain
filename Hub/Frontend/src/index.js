import React from 'react';
import ReactDOM from 'react-dom/client';

import './Static/Style/Bootstrap/bootstrap.min.css';
import './Static/Style/SmartCurtain.css';
import SmartCurtain from './SmartCurtain';


const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
	<React.StrictMode>
		<SmartCurtain key="main"/>
	</React.StrictMode>
);


// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals