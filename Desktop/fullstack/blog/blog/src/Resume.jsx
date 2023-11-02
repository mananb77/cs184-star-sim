import './Resume.css'
import Seal from './assets/seal.png'
const Resume = () => {
    return (
        <>
          <section className='whole'>
          <div className='header'>
            <h3> Computer Science @ Berkeley&nbsp;</h3>
            <img className='headerIcon' src={Seal}/>
          </div>
          <div>
            <ul># Technical Coursework
              <li>Structure and Interpretation of Computer Programs</li>
              <li>Structure Algorithms & Data Structures</li>
              <li>Discrete Mathematics and Probability Theory</li>
              <li>Computer Architecture</li>  
            </ul>
            <ul># Academic Involvements
              <li>BERKE1337 Cybersecurity Club</li>
              <li>Full Stack Development DeCal</li>
              <li>Linux System Admininistration DeCal</li>
            </ul>
          </div>
        </section>
        </>
    );
};

export default Resume;