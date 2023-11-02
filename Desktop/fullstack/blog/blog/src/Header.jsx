import './Header.css'
import InstagramLogo from './assets/instagram.png'
import LinkedInLogo from './assets/linkedin.png'
import GitHubLogo from './assets/github.png'

const Header = () => {
    return (
        <div className='container'>
          <h2>
            NAK
          </h2>

          <div className='links'>
            <a href='https://www.instagram.com/kryeeeee/'><img className='icons' src={InstagramLogo}/></a>
            <a href='https://www.linkedin.com/in/kiriratanakvong/'><img className='icons' src={LinkedInLogo}/></a>
            <a href='https://github.com/NakVong'><img className='icons' src={GitHubLogo}/></a>
          </div>
        </div>
    );
};

export default Header;